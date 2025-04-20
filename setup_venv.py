import subprocess
import sys
from pathlib import Path

def create_virtualenv():
    """Create a virtual environment."""
    venv_dir = Path(".venv")
    if not venv_dir.exists():
        print("Creating virtual environment...")
        subprocess.run([sys.executable, "-m", "venv", str(venv_dir)], check=True)
        print(f"Virtual environment created at {venv_dir}")
    else:
        print("Virtual environment already exists.")

def install_requirements():
    """Install dependencies from requirements.txt."""
    requirements_path = Path("python") / "requirements.txt"
    
    if requirements_path.exists():
        print("Installing dependencies from requirements.txt...")
        # Ensure to use the Python in the virtual environment
        python_exe = Path(".venv") / "Scripts" / "python.exe" if sys.platform == "win32" else Path(".venv") / "bin" / "python"
        subprocess.run([str(python_exe), "-m", "pip", "install", "--upgrade", "pip"], check=True)
        subprocess.run([str(python_exe), "-m", "pip", "install", "-r", str(requirements_path)], check=True)
        print("Dependencies installed.")
    else:
        print("No requirements.txt found in 'python' directory. Skipping dependency installation.")

def main():
    create_virtualenv()
    install_requirements()
    print("Setup complete.")
    print("\nTo activate the virtual environment, use the following command:")
    if sys.platform == "win32":
        print("  .\\.venv\\Scripts\\activate.bat  # Command Prompt")
        print("  .\\.venv\\Scripts\\Activate.ps1  # PowerShell")
    else:
        print("  source .venv/bin/activate  # Bash/Zsh")
    print("\nOnce activated, you can run your Python scripts within the virtual environment.")

if __name__ == "__main__":
    main()
