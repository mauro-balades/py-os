
OS_FOLDER=$1
OS_USERNAME=$2
OS_PASSWORD=$3
EXEC_PATH=$4

echo "creating dir $OS_FOLDER"
mkdir "$OS_FOLDER"
cd "$OS_FOLDER"

echo "creating var folder..."
mkdir var
echo "creating exec folder..."
mkdir "exec"
echo "creating user folder..."
mkdir user
echo "creating bin folder..."
mkdir bin

echo "creating user's folders"
cd user
mkdir "$OS_USERNAME"
cd ..

echo "installing requirements..."

sudo -B apt update
sudo apt install git -y
sudo apt install python3 -y
sudo apt install make -y
sudo apt install gcc -y
curl https://sh.rustup.rs -sSf | sh


echo "------------------------------------------------------------"

echo "installing apps..."
cd "exec"

git clone https://github.com/maubg-debug/texor "_texor"
cd "_texor"
cargo build --bin "texor"
mv ./target/debug/texor ../texor
cd ..
sudo rm -rf "_texor"

git clone https://github.com/maubg-debug/r-shell "r_shell"
cd "r_shell"
cargo build --bin "shell"
mv ./target/debug/shell ../cmd
cd ..
sudo rm -rf "r_shell"

cp -rf "$EXEC_PATH/apps/calculator" "./calculator"
cd "calculator"
make
cd ..
mv "./calculator/calc" "./calc"
sudo rm -rf calculator

cd ..

echo "------------------------------------------------------------"

echo "adding user info..."
cd var

