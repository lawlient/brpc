# the file is used to update config & data files
FROM amis_jarvis_ui

WORKDIR amis

RUN npm i -g serve

EXPOSE 3000

CMD /usr/local/bin/serve /amis/node_modules/amis/sdk && tail -f /dev/null

