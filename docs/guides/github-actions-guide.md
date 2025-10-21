# 🚀 How to Use GitHub Actions

## 📚 Index
- [Intro](#sec-intro)
- [Core Ideas](#sec-core-ideas)
- [Summary](#sec-summary)
- [Example: “Taskly”](#sec-example-taskly)
- [Links & tutorials](#sec-links)

---

<a id="sec-intro"></a>
## Intro

GitHub Actions lets you **automate** tasks in your repository.  
You describe **workflows** in YAML files inside `.github/workflows/`.  
When specific **events** happen (example, a comment is posted, code is pushed), Actions **run jobs** made of **steps**.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

- **Workflow**: a YAML file describing when and what to run.
- **on**: the event that triggers the workflow (example, `push`, `pull_request`, `issue_comment`).
- **permissions**: minimal access required (example, `issues: write` to post comments).
- **jobs**: units of work (each job runs on a fresh virtual machine).
- **runs-on**: which machine image to use (example, `ubuntu-latest`).
- **steps**: actions or shell commands run in order.
- **actions**: reusable steps (example, `actions/checkout`, `actions/github-script`).

---

<a id="sec-summary"></a>
## 🧩 Summary

1. **Event happens** → (example, someone comments on an issue).  
2. GitHub starts your **workflow** → runs your **job** on a **runner**.  
3. The job executes **steps** (reusable actions or shell commands).  
4. Your workflow can **comment**, **build**, **test**, **upload artifacts**, etc.

---

<a id="sec-example-taskly"></a>
## 📄 Example: “Taskly” — Post a Closure Template via Comment

This workflow listens to **issue comments**.  
If someone writes `/taskly` on an **Issue** (not a PR), the bot replies with a **closure summary template** so you can fill it and close the issue.

> File path: `.github/workflows/close-issue.yml`

```yaml
# Workflow name (shows in Actions tab)
name: Taskly Template

# When should this run? On new issue comments.
on:
  issue_comment:
    types: [created]

# Permissions needed:
# - issues: write -> to post a comment
# - contents: read -> safe default
permissions:
  issues: write
  contents: read

jobs:
  post-template:
    # Run this job on a GitHub-hosted Ubuntu VM
    runs-on: ubuntu-latest

    # Guard: only run if
    # 1) the comment belongs to an Issue (not a PR)
    # 2) the comment contains our trigger "/taskly"
    if: ${{ github.event.issue.pull_request == null && contains(github.event.comment.body, '/taskly') }}

    steps:
      # Use the github-script action to call the API with small JS
      - name: Post closure template
        uses: actions/github-script@v7
        with:
          script: |
            // Build the comment body line by line
            const lines = [
              '## ✅ Closure Summary',
              '',
              '**What was done**',
              '- ',
              '',
              '**How it was done (approach / steps)**',
              '- ',
              '',
              '**References**',
              '- Docs: ',
              '- Links: ',
            ];
            const body = lines.join('\n');

            // Post the comment to the same issue
            await github.rest.issues.createComment({
              ...context.repo,
              issue_number: context.payload.issue.number,
              body
            });
```
---

## 📝 What this workflow does

- on → issue_comment: triggers when a new comment is created.

- permissions: allows the workflow to write comments on issues.

- jobs.post-template:

- runs-on: uses an Ubuntu runner.

- if: only continues if the comment is on an Issue and includes /taskly.

- steps: runs actions/github-script to post a template comment.

---

## ▶️ How to use it

On any Issue, add a comment containing /taskly.
The bot will reply with the closure template → you fill it → close the issue.

---

<a id="sec-links"></a>

## Links

https://docs.github.com/en/actions/get-started/quickstart

https://www.freecodecamp.org/news/learn-to-use-github-actions-step-by-step-guide/#heading-github-actions-syntax
