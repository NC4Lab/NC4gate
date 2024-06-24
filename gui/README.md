# GUI

## Install Conda 
https://www.anaconda.com/download/success

## Install QT
Only needed if you are modifying the GUI
https://www.qt.io/download

## Development 
### In a conda-enabled terminal, create an environment with pyqt installed

- Open Anaconda terminal
- Go to the gui directory
```
cd gui
```
- Create the environment
```
conda env create -f nc4_mazegate_env.yml
```
- If needed you can remove the environemnt
```
conda remove --name nc4_mazegate_env --all
```

### Set the python interpreter to the conda environment.
- Go to Python: Select Interpreter
- Example
```
Python 3.12.3 ('nc4_mazegate_env')
```

### Add/remove 
- Go to the gui directory
```
cd gui
```
- Activate the environment
```
conda activate nc4_mazegate_env
```
- Add a package (example: numpy)
```
conda install numpy
```
- Remove a package (example: numpy)
```
conda uninstall numpy
```
- Export the environement to nc4_mazegate_env.yml
```
conda env export > nc4_mazegate_env.yml
```