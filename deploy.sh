install -m 0755 query_pcimap.wt /srv/http/harixB
rm /srv/http/harixB/queryDistro.sh
install -m 0755 queryDistro.sh /srv/http/harixB
cp -R resources /srv/http/harixB
