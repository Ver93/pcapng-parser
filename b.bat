gcc src/main.c ^
src/utils/utils.c ^
src/validation/validation.c ^
-Isrc ^
-Isrc/types ^
-Isrc/utils ^
-Isrc/validation ^
-Wall -Wextra -Wpedantic ^
-o bin/main.exe