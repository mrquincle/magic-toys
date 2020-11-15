# Magic toys

There are two tools in this repository:

* `magic_control`
* `magic_touch`

The tool `magic_control` expects a command like

```
 ./magic_control 90:e2:02:2c:6f:1e 50
```

It sends an intensity level to the magic device.

The tool `magic_touch` expects a command like

```
./magic_touch 90:e2:02:2c:6f:1e
```

It sends an intensity value inverse related to the distance of the device with the machine you run the tool from.

# Build

Fork this repository, run git clone and cd to the directory of the downloaded code.

```
mkdir build
cd build
cmake ..
make
```

# Copyrights

Code has been from everywhere, please check the authors. 
