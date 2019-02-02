# S18 Networks – week 2 lab report

Innopolis University, 2019
B17-03

Gleb Petrakov
g.petrakov@innopolis.ru


## Architecture

**Host system:** Mac Os X Mojave 10.14.3
**Virtualization:** VMware Fusion 11

**Base image:** Ubuntu Server 18.10 64-bit
**Linked images:** client1, client2, router

## Goal

Create network topology (star topology), which allows `client1` and `client2` to communicate with each other via `router`.

## Configuration

In Ubuntu Server 18.10 interface configuration was moved to `netplan`. 
Configuration file: /etc/netplan/50-cloud-init.yaml
Configuration file format:
```
network:
    ethernets:
        <interface>:
            addresses: [<ip>/<mask>]
            gateway4: <gateway>
            dhcp4: false
    version: 2
```

### client1

**ens33**
IP: `192.168.1.1/24`
Gateway: `192.168.1.10/24`


### client2

**ens33**
IP: `192.168.2.1/24`
Gateway: `192.168.2.10/24`


### router

*Enabled ipv4 forwarding.*
*Enabled routing within interfaces with `ufw` (uncomplicated firewall).*

**ens33**
IP: `192.168.1.10/24`
Gateway: `0.0.0.0`

**ens38**
IP: `192.168.2.10/24`
Gateway: `0.0.0.0`

## Result

Network created, netplan configured, routing configured.

`client1` is able to send ping-requests to `client2` (and vice versa) without direct connection, but through `router`.

---

![router](https://i.imgur.com/dfU4eeM.png)
*router*

---

![client1](https://i.imgur.com/C70mT7F.png)
*client1*

---

![client2](https://i.imgur.com/7IjIBgh.png)
*client2*



