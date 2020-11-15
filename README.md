# Magic toys

There are three tools in this repository:

* `magic_control`
* `magic_touch`
* `magic_remote`

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

The tool `magic_remote` expects a command like

```
./magic_remote 90:e2:02:2c:6f:1e
```

It uses the website http://control-me.herokuapp.com/ to retrieve the intensity. Anyone can access that website!

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
