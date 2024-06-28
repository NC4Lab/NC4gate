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
conda env create -f nc4_mazegate_env.yml
```
- If needed you can remove the environment
```
conda remove --name nc4_mazegate_env --all
```

### Activate the environment using the terminal
- Open Anaconda terminal ("Anaconda Prompt")
- Go to the gui directory
```
cd gui
```
- Activate the environment
```
conda activate nc4_mazegate_env
```

### Activate the environment using the VS Code
- Go to:
```
Python: Select Interpreter
```
- Example:
```
Python 3.12.3 ('nc4_mazegate_env')
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
- Export the environement to nc4_mazegate_env.yml
```
conda env export > nc4_mazegate_env.yml
```