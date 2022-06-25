# u-boot-ssc335
U-Boot for Infinity6xx SoC's

## Building
```export ARCH=arm```

```export CROSS_COMPILE=/opt/arm-linux-gnueabihf-4.8.3-201404/bin/arm-linux-gnueabihf- ```

```make infinity6b0_defconfig```

```make -j5```

```./make_boot.sh (optionally)```
