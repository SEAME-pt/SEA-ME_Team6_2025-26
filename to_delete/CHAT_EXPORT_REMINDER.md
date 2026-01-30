# Lembrete: Exportar Sessões do Chat Copilot

Para evitar perder o histórico de conversas após reinstalações do VSCode ou Copilot, faz o seguinte **todos os dias**:

1. Com o Copilot Chat aberto no VSCode.
2. Pressiona `Command + Shift + P` (Mac) ou `Ctrl + Shift + P` (Windows/Linux).
3. Digita "Chat: Export Session".
4. Seleciona e guarda o ficheiro como `.json` na raiz deste projeto (ex: `chat-session-YYYY-MM-DD.json`).
5. Para importar noutra instalação: `Command + Shift + P` > "Chat: Import Session" > seleciona o ficheiro.

**Nota:** Estes ficheiros estão no `.gitignore` para não serem enviados para o GitHub.