# client-server hardware monitor

## dependences

1. zmq/zmqpp 
2. pistache
3. sodium

## build

```shell
./build.sh
```

## usage / test case

*client side:*
```shell
./build/hmoncli
```

*server side:*
```shell

./build/hmonsrv 127.0.0.1 #or client's IP in case of 2 peers
curl 127.0.0.1:9080 
```

## short notes

- **s** command + Enter stops hardware, to enable it again we need to implement a **r** command, for example
- sodium (with ed25519) was used due to the fact that it's already exists in the zmq library
