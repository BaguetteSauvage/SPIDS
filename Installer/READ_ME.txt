Pour que l'écran soit dans le bon sens:
$ sudo sh -c "echo 'lcd_rotate=2' >> /boot/config.txt"

Pour installer node-red:
$ sudo npm install -g --unsafe-perm node-red

Pour lancer le script on startup:
$ nano /home/pi/.config/lxsession/LXDE-pi/autostart
Puis ajouter la ligne
@sudo bash /home/pi/on_startup.sh
À ajuster selon l'adresse et le nom du .sh

