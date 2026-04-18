# KUKSA Databroker Security: TLS and JWT

This document summarizes the work done to secure the **Eclipse KUKSA Databroker** in our project using **TLS** for transport security and **JWT** for access control and authorization.

The setup applies to the following architecture:

* **Raspberry Pi 5**: runs KUKSA Databroker and the publisher service
* **Raspberry Pi 4**: runs a remote reader client that subscribes to vehicle data

---

## Index

- [1. Brief Concepts Overview](#1-brief-concepts-overview)
  - [1.1 TLS (Transport Layer Security)](#tls-transport-layer-security)
  - [1.2 JWT (JSON Web Token)](#jwt-json-web-token)
- [2. TLS Implementation in the Project](#2-tls-implementation-in-the-project)
  - [2.7 TLS Permission Issue (Problem & Fix)](#27-tls-permission-issue-problem--fix)
- [3. JWT Authorization Implementation](#3-jwt-authorization-implementation)
- [4. Final Result](#4-final-result)

---

## 1. Brief Concepts Overview

### TLS (Transport Layer Security)

TLS is used to secure the communication channel between KUKSA and its clients. It provides:

* Encryption of data in transit
* Server authentication (clients verify they are talking to the correct databroker)
* Integrity (data cannot be modified in transit)

In this project, TLS is enabled directly in the KUKSA Databroker using a locally generated Certificate Authority (CA) and a server certificate.

---

### JWT (JSON Web Token)

JWT is used for **authorization**, not encryption. It allows KUKSA to decide:

* Who the client is
* What data the client can read or write

Clients authenticate by sending a signed JWT in every gRPC request using the `Authorization: Bearer <token>` header. Permissions are defined in the `scope` claim of the token.

---

## 2. TLS Implementation in the Project

### 2.1 Create TLS directory

```bash
sudo mkdir -p /etc/kuksa/tls
sudo chmod 700 /etc/kuksa/tls
```

---

### 2.2 Generate a local CA and server certificate

```bash
cd /etc/kuksa/tls

# Root CA
sudo openssl genrsa -out ca.key 4096
sudo openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 \
  -subj "/CN=KUKSA-Local-CA" \
  -out ca.crt

# Server key
sudo openssl genrsa -out server.key 4096

# Server CSR
sudo openssl req -new -key server.key \
  -subj "/CN=our.rpi5.ip" \
  -out server.csr

# Subject Alternative Name (SAN)
cat | sudo tee server.ext >/dev/null <<'EOF'
subjectAltName=IP:our.rpi5.ip
extendedKeyUsage=serverAuth
keyUsage=digitalSignature,keyEncipherment
EOF

# Sign server certificate
sudo openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial \
  -out server.crt -days 825 -sha256 -extfile server.ext

sudo chmod 600 server.key
```

---

### 2.3 Configure KUKSA Databroker to use TLS

Edit `/etc/default/kuksa-databroker`:

```bash
cat > /etc/default/kuksa-databroker <<'EOF'
EXTRA_ARGS="--address 0.0.0.0 --port 55555 --vss /etc/kuksa/vss.json \
--tls-cert /etc/kuksa/tls/server.crt --tls-private-key /etc/kuksa/tls/server.key \
--disable-authorization"
EOF
```

---

### 2.4 Restart the service

```bash
sudo systemctl daemon-reload
sudo systemctl restart kuksa-databroker
sudo systemctl status kuksa-databroker --no-pager
```

---

### 2.5 Test TLS from a client

Copy the CA certificate to the RPi4 and test:

```bash
kuksa-client --cacertificate /etc/kuksa/tls/ca.crt grpcs://our.rpi5.ip:55555
```

---

### 2.6 Update gRPC channel creation in code

**Before (insecure):**

```cpp
auto channel = grpc::CreateChannel(host_port,
    grpc::InsecureChannelCredentials());
```

**After (TLS):**

```cpp
auto channel = grpc::CreateChannel(host_port,
    grpc::SslCredentials(ssl_opts));
```

---

### 2.7 TLS Permission Issue (Problem & Fix)

**Problem:**

```
Process: ExecStart=/usr/bin/databroker ... (status=1/FAILURE)
```

Cause: databroker runs without root privileges and cannot read `server.key`.

**Fix:**

```bash
sudo chown root:kuksa /etc/kuksa/tls/server.key
sudo chmod 640 /etc/kuksa/tls/server.key
sudo chmod 644 /etc/kuksa/tls/server.crt /etc/kuksa/tls/ca.crt
```

---

## 3. JWT Authorization Implementation

### 3.1 Generate JWT keypair (RPi5)

```bash
sudo mkdir -p /etc/kuksa/jwt
cd /etc/kuksa/jwt

# Private key (kept secret)
sudo openssl genrsa -out jwt_private.pem 2048

# Public key (used by databroker)
sudo openssl rsa -in jwt_private.pem -pubout -out jwt_public.pem

sudo chmod 600 jwt_private.pem
sudo chmod 644 jwt_public.pem
```

---

### 3.2 Enable authorization in KUKSA Databroker

```bash
cat > /etc/default/kuksa-databroker <<'EOF'
EXTRA_ARGS="--address 0.0.0.0 --port 55555 \
--vss /etc/kuksa/vss.json \
--tls-cert /etc/kuksa/tls/server.crt --tls-private-key /etc/kuksa/tls/server.key \
--jwt-public-key /etc/kuksa/jwt/jwt_public.pem"
EOF
```

Restart:

```bash
sudo systemctl daemon-reload
sudo systemctl restart kuksa-databroker
sudo systemctl status kuksa-databroker --no-pager
```

---

### 3.3 Mint a JWT with scope (RPi5)

Install dependencies:

```bash
python3 -m pip install --user pyjwt cryptography
```

`mint_jwt.py`:

```python
#!/usr/bin/env python3

import time
import jwt

PRIVATE_KEY_PATH = "/etc/kuksa/jwt/jwt_private.pem"

def mint() -> str:
    now = int(time.time())

    payload = {
        "sub": "rpi4-reader",
        "iss": "seame-team6",
        "aud": ["kuksa.val"],
        "iat": now,
        "exp": now + 60*60*24*240,
        "scope": "read:Vehicle.*"
    }

    with open(PRIVATE_KEY_PATH, "rb") as f:
        priv = f.read()

    token = jwt.encode(payload, priv, algorithm="RS256")
    return token.decode("utf-8") if isinstance(token, bytes) else token

if __name__ == "__main__":
    print(mint())
```

Generate and distribute token:

```bash
sudo python3 mint_jwt.py > /tmp/reader.jwt
scp /tmp/reader.jwt team6qt@our.rpi4.ip:/etc/kuksa/reader.jwt
```

⚠️ A separate JWT **must also be created for the publisher** on the RPi5, using a `provide:` scope.

---

### 3.4 Send JWT from the reader client (RPi4)

⚠️ Important: In the RPi5 it's also necessary to change the publish code to publish with the ctx metadata.

In the gRPC client code:

```cpp
int main(int argc, char** argv)
{
    grpc::ClientContext ctx;
    ctx.AddMetadata("authorization", "Bearer " + jwt);

    std::unique_ptr<grpc::ClientReader<kuksa::val::v2::SubscribeResponse>> stream(
        stub->Subscribe(&ctx, req)
    );
}
```

Each gRPC call must include the JWT in the metadata.

---

## 4. Final Result

* All KUKSA communication is encrypted using TLS
* Databroker identity is verified by clients
* Access to vehicle data is restricted using JWT scopes
* Reader and publisher are isolated by permissions

This setup provides a secure, scalable foundation aligned with automotive security best practices.
