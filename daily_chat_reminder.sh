#!/bin/bash

# Daily reminder to export Copilot chat sessions
echo "🔔 LEMBRETE DIÁRIO: Exportar Sessões do Chat Copilot"
echo ""
echo "Para não perder o histórico:"
echo "1. Copilot Chat aberto no VSCode"
echo "2. Command + Shift + P > 'Chat: Export Session'"
echo "3. Guardar como chat-session-YYYY-MM-DD.json na raiz do projeto"
echo ""
echo "Ficheiros já ignorados no .gitignore."
echo ""
# Optional: Show notification if terminal-notifier is installed
if command -v terminal-notifier >/dev/null 2>&1; then
    terminal-notifier -title "Copilot Chat Export" -message "Não esqueças de exportar a sessão do chat hoje!"
fi