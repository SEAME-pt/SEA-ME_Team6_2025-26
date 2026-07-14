#!/usr/bin/env python3
"""
videostream.py — MJPEG live stream wrapper para scripts de inferência

Uso:
    python3 videostream.py <script.py> [args do script...]

Exemplos:
    python3 videostream.py inference.py
    python3 videostream.py inference.py 120
    python3 videostream.py inference_tusimple_v3_new.py 60 --save
    python3 videostream.py inference_lka.py

Abre no browser do laptop:  http://10.21.220.191:8081/

O script de inferência corre sem alterações. O videostream.py:
  1. Aplica um patch em memória que move a geração do frame anotado
     para fora do bloco "if save_video", tornando-a sempre activa.
  2. Injeta _push_stream() no namespace do script.
  3. Serve os frames via MJPEG HTTP na porta 8081.
"""

import sys
import re
import cv2
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer

PORT = 8081

# ── MJPEG server ──────────────────────────────────────────────────────────────
_stream_jpg  = None
_stream_lock = threading.Lock()


class _MJPEGHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path not in ('/', '/stream'):
            self.send_response(404)
            self.end_headers()
            return
        self.send_response(200)
        self.send_header('Content-Type', 'multipart/x-mixed-replace; boundary=frame')
        self.end_headers()
        try:
            while True:
                with _stream_lock:
                    jpg = _stream_jpg
                if jpg is not None:
                    self.wfile.write(
                        b'--frame\r\nContent-Type: image/jpeg\r\n\r\n' +
                        jpg + b'\r\n')
                import time as _t
                _t.sleep(0.05)   # ~20 fps max para o viewer
        except (BrokenPipeError, ConnectionResetError):
            pass

    def log_message(self, *_):
        pass


def _push_stream(frame):
    """Codifica o frame como JPEG e disponibiliza-o para o servidor MJPEG."""
    _, jpg = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 70])
    global _stream_jpg
    with _stream_lock:
        _stream_jpg = jpg.tobytes()


def _start_mjpeg_server(port=PORT):
    """Arranca o servidor MJPEG. Se já estiver a correr, não faz nada."""
    pass  # gerido pelo videostream.py antes do exec — no-op aqui


# ── Patch dinâmico ────────────────────────────────────────────────────────────
def _patch(src: str) -> str:
    """
    Transforma o bloco 'if save_video and async_writer is not None:' de:

        if save_video and async_writer is not None:
            frame_out = draw_X(frame.copy(), ...)
            frame_out = draw_overlay(frame_out, ...)
            async_writer.write(frame_out)

    para:

        frame_out = draw_X(frame.copy(), ...)
        frame_out = draw_overlay(frame_out, ...)
        _push_stream(frame_out)
        if save_video and async_writer is not None:
            async_writer.write(frame_out)

    Funciona para qualquer nível de indentação e qualquer número de
    linhas de construção do frame_out.
    """
    lines = src.split('\n')
    out   = []
    i     = 0

    while i < len(lines):
        line    = lines[i]
        stripped = line.lstrip()
        base_indent  = len(line) - len(stripped)
        inner_indent = base_indent + 4

        if stripped == 'if save_video and async_writer is not None:':
            # Recolhe o corpo do bloco if
            body = []
            i += 1
            while i < len(lines):
                bl = lines[i]
                bs = bl.lstrip()
                if bs and (len(bl) - len(bs)) <= base_indent:
                    break    # fim do bloco
                body.append(bl)
                i += 1

            # Separa as linhas de construção do frame_out da linha de escrita
            write_line  = None
            frame_lines = []
            for bl in body:
                if re.search(r'async_writer\.write\(', bl):
                    write_line = bl
                else:
                    # Desindenta um nível (4 espaços)
                    if bl[:inner_indent] == ' ' * inner_indent:
                        frame_lines.append(' ' * base_indent + bl[inner_indent:])
                    else:
                        frame_lines.append(bl)

            # Remove linhas vazias no fim (artefactos do bloco original)
            while frame_lines and not frame_lines[-1].strip():
                frame_lines.pop()

            # Emite as linhas transformadas
            out.extend(frame_lines)
            out.append(' ' * base_indent + '_push_stream(frame_out)')
            out.append(' ' * base_indent + 'if save_video and async_writer is not None:')
            if write_line:
                out.append(write_line)
            continue   # i já foi avançado dentro do loop de body

        out.append(line)
        i += 1

    patched = '\n'.join(out)

    # Verifica que o patch encontrou e transformou o bloco
    if '_push_stream(frame_out)' not in patched:
        print('[videostream] AVISO: bloco "if save_video" não encontrado — '
              'o script pode não seguir o padrão esperado. '
              'O stream pode não mostrar frames anotados.')

    return patched


# ── Entry point ───────────────────────────────────────────────────────────────
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    script_path = sys.argv[1]
    # Ajusta argv para que o script veja os seus próprios argumentos
    sys.argv = [script_path] + sys.argv[2:]

    # Arranca o servidor MJPEG antes de correr o script
    server = HTTPServer(('0.0.0.0', PORT), _MJPEGHandler)
    threading.Thread(target=server.serve_forever, daemon=True, name='mjpeg').start()
    print(f'[STREAM] http://10.21.220.191:{PORT}/  (abre no browser do laptop)\n')

    # Lê e patcha o script em memória
    with open(script_path) as f:
        src = f.read()
    src = _patch(src)

    # Corre o script com _push_stream e _start_mjpeg_server injectados
    g = {
        '__name__'           : '__main__',
        '__file__'           : script_path,
        '_push_stream'       : _push_stream,
        '_start_mjpeg_server': _start_mjpeg_server,  # no-op
        '_stream_jpg'        : _stream_jpg,
        '_stream_lock'       : _stream_lock,
        '_MJPEGHandler'      : _MJPEGHandler,
        'BaseHTTPRequestHandler': BaseHTTPRequestHandler,
        'HTTPServer'         : HTTPServer,
    }
    exec(compile(src, script_path, 'exec'), g)
