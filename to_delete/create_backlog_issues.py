import requests
import os
from dotenv import load_dotenv

# Load environment variables
load_dotenv()
TOKEN = os.getenv("GITHUB_TOKEN")
REPO = os.getenv("GITHUB_REPO")

HEADERS = {
    "Authorization": f"token {TOKEN}",
    "Accept": "application/vnd.github+json"
}

# --- Product Backlog Items ---
backlog = [
    {
        "id": "PB-1",
        "title": "Assemble PiRacer Robot",
        "body": "As a team member, I want to assemble the PiRacer robot so that it can power on and interface with motors and the display.\n\n**Acceptance Criteria:**\n- PiRacer hardware assembled and boots correctly.\n- All components (motors, display, Hailo AI Hat) detected and functional.\n\n**L0 Link:** L0-1",
        "labels": ["Epic: Hardware Assembly", "Priority: Must"]
    },
    {
        "id": "PB-2",
        "title": "Boot Raspberry Pi with Linux + Qt",
        "body": "As a developer, I want to boot the Raspberry Pi 5 with a Linux environment that supports Qt so I can develop GUI applications.\n\n**Acceptance Criteria:**\n- Raspberry Pi boots without errors.\n- Qt tools (qmake, QtCreator) are installed and runnable.\n\n**L0 Link:** L0-2",
        "labels": ["Epic: Hardware Assembly", "Priority: Must"]
    },
    # Add PB-3 → PB-15 here similarly...
]

# --- Create issues on GitHub ---
def create_issue(item):
    url = f"https://api.github.com/repos/{REPO}/issues"
    data = {
        "title": f"{item['id']} - {item['title']}",
        "body": item["body"],
        "labels": item["labels"]
    }

    response = requests.post(url, headers=HEADERS, json=data)
    if response.status_code == 201:
        print(f"✅ Created issue: {item['id']}")
    else:
        print(f"❌ Failed to create {item['id']}: {response.status_code} {response.text}")

def main():
    for item in backlog:
        create_issue(item)

if __name__ == "__main__":
    main()
