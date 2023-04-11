FROM node:latest

RUN git clone --depth=1 https://github.com/baidu/amis.git

WORKDIR amis

RUN npm i --legacy-peer-deps && npm run build && npm i amis 

COPY ./*.html /amis/node_modules/amis/sdk/

RUN npm i -g serve

EXPOSE 3000

CMD /usr/local/bin/serve /amis/node_modules/amis/sdk && tail -f /dev/null

