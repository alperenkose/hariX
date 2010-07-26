DEPLOY_HARIX=/var/www/harix
install -m 0755 harix.wt $DEPLOY_HARIX
rm $DEPLOY_HARIX/queryDistro.sh
install -m 0755 queryDistro.sh $DEPLOY_HARIX
cp -R resources $DEPLOY_HARIX
