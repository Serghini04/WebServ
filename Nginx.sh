# Install Brew : 
git clone https://github.com/Homebrew/brew ~/.homebrew

echo 'export PATH="$HOME/.homebrew/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

brew update

# Now Install NGINX

brew install openssl@3
brew link openssl@3
brew install nginx

# to get config file
code /Users/meserghi/.homebrew/etc/nginx/nginx.conf

# Do this if change something in nginx.conf 
nginx -s reload

# to Check Space memory :
du -sh ~/*