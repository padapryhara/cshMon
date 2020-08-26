# client-server hardware monitor

## dependences

1. zmq/zmqpp 
2. ~~pistache~~
3. ~~sodium~~ (not included to this revision due to unstable behavior)
## build

```shell
./build.sh
```

## usage

```shell
./build/hmoncli
./build/hmonsrv 127.0.0.1
```

## short notes

- **s** command + Enter stops hardware, to enable it again we need to implement a **r** command, for example
- sodium was used due to the fact that it's already exists in the zmq library
