To play the game you need to be on Linux environment.
If you are using WSL2, download VcXsrv (https://sourceforge.net/projects/vcxsrv/) and update your .bashrc file with the following lines:
```
export DISPLAY=$(ip route list default | awk '{print $3}'):0
export LIBGL_ALWAYS_INDIRECT=1
```
To access the file type ``nano ~/.bashrc`` and paste the lines above at the end of the file. Then, save and exit.
Then, run the following commands in programs/game directory:

``make run`` if you want to run the game with a Vector based data structure (slower when platforms increase)

``make run-alt`` if you want to run the game with a Set based data structure.

Note: You might need to run `bash config.sh` if some libraries are missing.

