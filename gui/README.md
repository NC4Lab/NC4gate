# GUI

## Install Conda 
https://www.anaconda.com/download/success

## Install QT
Only needed if you are modifying the GUI
https://www.qt.io/download

## Setup

### Create an environment which will install the needed packages
- Open Anaconda terminal ("Anaconda Prompt")
- Go to the gui directory
```
cd gui
```
- Create the environment
```
conda env create -f nc4gate_env.yml
```
- If needed you can remove the environment
```
conda remove --name nc4gate_env --all
```

### Activate the environment using the terminal
- Open Anaconda terminal ("Anaconda Prompt")
- Go to the gui directory
```
cd gui
```
- Activate the environment
```
conda activate nc4gate_env
```

### Activate the environment using the VS Code
- Go to:
```
Python: Select Interpreter
```
- Example:
```
Python 3.12.3 ('nc4gate_env')
```

## Development 
- Add a package (example: numpy)
```
conda install numpy
```
- Remove a package (example: numpy)
```
conda uninstall numpy
```
- Export the environement to nc4gate_env.yml
```
conda env export > nc4gate_env.yml
```

# Hardware

## I2C addressing using the NC4 Cypress Board dip switch

| A1 | A2 | A3 | A4 | A5 | A6 | Bin      | Hex   |
|----|----|----|----|----|----|----------|-------|
| 1  | 0  | 0  | 0  | 0  | 0  | 00000010 | 0x2   |
| 0  | 1  | 0  | 0  | 0  | 0  | 00000100 | 0x4   |
| 1  | 1  | 0  | 0  | 0  | 0  | 00000110 | 0x6   |
| 0  | 0  | 1  | 0  | 0  | 0  | 00001000 | 0x8   |
| 1  | 0  | 1  | 0  | 0  | 0  | 00001010 | 0xA   |
| 0  | 1  | 1  | 0  | 0  | 0  | 00001100 | 0xC   |
| 1  | 1  | 1  | 0  | 0  | 0  | 00001110 | 0xE   |
| 0  | 0  | 0  | 1  | 0  | 0  | 00010000 | 0x10  |
| 1  | 0  | 0  | 1  | 0  | 0  | 00010010 | 0x12  |
| 0  | 1  | 0  | 1  | 0  | 0  | 00010100 | 0x14  |
| 1  | 1  | 0  | 1  | 0  | 0  | 00010110 | 0x16  |
| 0  | 0  | 1  | 1  | 0  | 0  | 00011000 | 0x18  |
| 1  | 0  | 1  | 1  | 0  | 0  | 00011010 | 0x1A  |
| 0  | 1  | 1  | 1  | 0  | 0  | 00011100 | 0x1C  |
| 1  | 1  | 1  | 1  | 0  | 0  | 00011110 | 0x1E  |
| 0  | 0  | 0  | 0  | 1  | 0  | 00100000 | 0x20  |
| 1  | 0  | 0  | 0  | 1  | 0  | 00100010 | 0x22  |
| 0  | 1  | 0  | 0  | 1  | 0  | 00100100 | 0x24  |
| 1  | 1  | 0  | 0  | 1  | 0  | 00100110 | 0x26  |
| 0  | 0  | 1  | 0  | 1  | 0  | 00101000 | 0x28  |
| 1  | 0  | 1  | 0  | 1  | 0  | 00101010 | 0x2A  |
| 0  | 1  | 1  | 0  | 1  | 0  | 00101100 | 0x2C  |
| 1  | 1  | 1  | 0  | 1  | 0  | 00101110 | 0x2E  |
| 0  | 0  | 0  | 1  | 1  | 0  | 00110000 | 0x30  |
| 1  | 0  | 0  | 1  | 1  | 0  | 00110010 | 0x32  |
| 0  | 1  | 0  | 1  | 1  | 0  | 00110100 | 0x34  |
| 1  | 1  | 0  | 1  | 1  | 0  | 00110110 | 0x36  |
| 0  | 0  | 1  | 1  | 1  | 0  | 00111000 | 0x38  |
| 1  | 0  | 1  | 1  | 1  | 0  | 00111010 | 0x3A  |
| 0  | 1  | 1  | 1  | 1  | 0  | 00111100 | 0x3C  |
| 1  | 1  | 1  | 1  | 1  | 0  | 00111110 | 0x3E  |
| 0  | 0  | 0  | 0  | 0  | 1  | 01000000 | 0x40  |
| 1  | 0  | 0  | 0  | 0  | 1  | 01000010 | 0x42  |
| 0  | 1  | 0  | 0  | 0  | 1  | 01000100 | 0x44  |
| 1  | 1  | 0  | 0  | 0  | 1  | 01000110 | 0x46  |
| 0  | 0  | 1  | 0  | 0  | 1  | 01001000 | 0x48  |
| 1  | 0  | 1  | 0  | 0  | 1  | 01001010 | 0x4A  |
| 0  | 1  | 1  | 0  | 0  | 1  | 01001100 | 0x4C  |
| 1  | 1  | 1  | 0  | 0  | 1  | 01001110 | 0x4E  |
| 0  | 0  | 0  | 1  | 0  | 1  | 01010000 | 0x50  |
| 1  | 0  | 0  | 1  | 0  | 1  | 01010010 | 0x52  |
| 0  | 1  | 0  | 1  | 0  | 1  | 01010100 | 0x54  |
| 1  | 1  | 0  | 1  | 0  | 1  | 01010110 | 0x56  |
| 0  | 0  | 1  | 1  | 0  | 1  | 01011000 | 0x58  |
| 1  | 0  | 1  | 1  | 0  | 1  | 01011010 | 0x5A  |
| 0  | 1  | 1  | 1  | 0  | 1  | 01011100 | 0x5C  |
| 1  | 1  | 1  | 1  | 0  | 1  | 01011110 | 0x5E  |
| 0  | 0  | 0  | 0  | 1  | 1  | 01100000 | 0x60  |
| 1  | 0  | 0  | 0  | 1  | 1  | 01100010 | 0x62  |
