 Jarvis

Jarvis is my good assistant. He helps me to manager my work and life.

## Core Functions

1. [Financial Management](doc/01Financial.md)
2. [Login](doc/02Login.md)
3. [Task Management](doc/03Task.md)

## Fix Me

### Maintain manually

~~~Creating image & starting contianer and so on are too complex to maintain.~~~

~~~There are too many details to remember it with brain.~~~

~~~We need config file or scripts to maintain all of it.~~~

~~~`docker-compose` maybe a good option.~~~

docker-compose helps me to manage containers easily.

It's a yaml config file, remember something like port, volume, network, name and so on.

What I need to do is execute command: `docker-compose up -d --build`, it is easy.

### Bugs cause of windows + docker + wsl2 enviorment

~~~Need a server machine run ubuntu os.~~~

~~~Not enmergency, I will have it in the future.~~~

~~~Eventually，we can access to the system no matter where and no matter when.~~~

Since my old machine have worked for me 5 years and disk is broken now, I choose to buy a new machine.

Then I transform the old one to be a ubuntu server.

For now it works well, and I don't have to solve problems with windows and wsl.

### Data is not persistence

Database & table exists in db container, data will lost after container deletion.

The thoroughly solution is deploy redundancy. It means more than one machine is needed.

For now the system deployment should be as simple as possible.

~~We can start a cron event to dump db data and sync to another directory.~~

Actually，mount mysql container /var/lib/mysql into host device will keep all data.

Data is still lost if the host filesystem is broken.

### MYSQL Chinese Coding Error

[fix mysql chinese coding error](https://blog.csdn.net/m0_38023584/article/details/105584623)
