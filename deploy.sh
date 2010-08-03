DEPLOY_HARIX=/srv/http/harix
# rm -r $DEPLOY_HARIX
mkdir -p $DEPLOY_HARIX
install -m 0755 harix.wt $DEPLOY_HARIX
rm -f $DEPLOY_HARIX/queryDistro.sh
install -m 0755 queryDistro.sh $DEPLOY_HARIX
cp -R resources $DEPLOY_HARIX
cp -R doc $DEPLOY_HARIX