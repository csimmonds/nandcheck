# nandcheck
Checks the status of NAND flash chips using the Linux MTD interface

# Usage
Just point it at the MTD device node:
```
nand_check /dev/mtdN
```

For each NAND eraseblock, nand_check prints

|----|------------------------|
| B  | Bad block              |
| .  | Empty                  |
| -  | Partially filled       |
| =  | Full, no summary node  |
| S  | Full, summary node     |
 
# Compiling
When cross-compiling, set CC to point to your cross compiler, for example:
```
make CC=arm-linux-gnueabihf-gcc
```

# Further information
You may want to take a look at my blog on [Reducing JFFS2 mount time]( http://www.2net.co.uk/tutorial/jffs2-summary)
