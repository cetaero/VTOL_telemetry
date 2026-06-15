A simple git feature workflow has to be followed to ensure clean codebase.

git clone <repo-url>
cd <project-folder>

Everytime you start a new task

git checkout main
git pull origin main
git checkout -b feature/your_task_name

eg: git checkout -b feature/wifi-receive


While working

git add .
git commit -m "commit"

Please add relevant comment messages.


When task is done

git push origin feature/task-name

Please never push directly to main branch!!! 


