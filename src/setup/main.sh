
OS_FOLDER=$1
OS_USERNAME=$2
OS_PASSWORD=$3

echo "creating dir $OS_FOLDER"
mkdir "$OS_FOLDER"
cd "$OS_FOLDER"

echo "creating var folder..."
mkdir var
echo "creating exec folder..."
mkdir exec
echo "creating user folder..."
mkdir user
echo "creating bin folder..."
mkdir bin

echo "creating user's folders"
cd user
mkdir "$OS_USERNAME"
cd ..

echo "installing requirements..."

sudo apt update
sudo apt install git -y
sudo apt install python3 -y
