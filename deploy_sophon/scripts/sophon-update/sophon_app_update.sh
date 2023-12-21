#sudo cd /tmp/
#sudo mkdir updatetemp
#sudo cd updatetemp
##download
sudo tar xvf sophon-update.tar
sudo mv sophon-sail /opt/sophon
sudo mv paho-mqtt /opt/sophon
sudo mv etc/profile.d/sophon-mqtt-rpath.sh /etc/profile.d/
sudo mv etc/profile.d/sophon-sail-rpath.sh /etc/profile.d/

sudo cd /tmp/
sudo rm -rf updatetemp
echo "update complete!"