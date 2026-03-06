import time
import sys

# \033[2J  = Clear the whole screen
# \033[H   = Move cursor to top-left (Home)
sys.stdout.write("\033[2J\033[H")

try:
    for i in range(1, 11):
        # \033[5;10H = Move cursor to Row 5, Column 10
        # \033[1;34m = Start Bold Blue text
        # \033[0m    = Reset all formatting
        sys.stdout.write(f"\033[5;10HProgress: [\033[1;34m{i * 10}%\033[0m] ")
        sys.stdout.flush() 
        time.sleep(0.5)
finally:
    # Move cursor down so the next prompt doesn't overwrite our work
    print("\n\n\nDone!")
