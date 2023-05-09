To play the game you need to be on Linux environment.
If you are using WSL2, download VcXrxv (https://sourceforge.net/projects/vcxsrv/) and update your .bashrc file with the following lines:
```
export DISPLAY=$(ip route list default | awk '{print $3}'):0
export LIBGL_ALWAYS_INDIRECT=1
```
Then, run the following commands in programs/game directory:

``make run`` if you want to run the game wih a Vector based data structure (slower when platforms increase)

``make run_-alt`` if you want to run the game wih a Set based data structure.
