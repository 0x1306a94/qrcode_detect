#!/bin/bash -e

cd $(dirname $0)

if [[ `uname` == 'Darwin' ]]; then
  MAC_REQUIRED_TOOLS="0x1306a94/tap/depctl"
  for TOOL in ${MAC_REQUIRED_TOOLS[@]}; do
    if [ ! $(which $TOOL) ]; then
      if [ ! $(which brew) ]; then
        echo "Homebrew not found. Trying to install..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
      fi
      echo "$TOOL not found. Trying to install..."
      brew install $TOOL
    fi
  done

elif [[ `uname` == 'Linux' ]]; then

  if [ ! -d "install_tools/depctl" ]; then
    mkdir -p install_tools/depctl
  fi

  wget https://github.com/0x1306a94/depctl/releases/download/v1.4.7/depctl-linux-x86_64.tar.gz -O install_tools/depctl-linux-x86_64.tar.gz
  tar -xzf install_tools/depctl-linux-x86_64.tar.gz -C install_tools/depctl
  chmod +x install_tools/depctl/depctl
  
fi