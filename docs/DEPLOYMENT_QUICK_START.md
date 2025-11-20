# Quick Deploy to Trim UI (5 Minutes)

## TL;DR - One Command Deploy

```bash
# Make sure you know your device IP first (check Knulli settings)
./deploy.sh 192.168.1.100 root
```

Done! Your app is installed and ready to test.

---

## Step-by-Step (If Script Doesn't Work)

### 1️⃣ Get Your Device IP
- Power on Trim UI
- Knulli Menu → System Settings → Network
- Note the IP address (e.g., `192.168.1.100`)

### 2️⃣ Test SSH Connection
```bash
ssh root@192.168.1.100
# Type: root
# Should connect successfully
exit
```

### 3️⃣ Build Locally
```bash
make build
# Binary created at: build/bin/knulli-input-tester
```

### 4️⃣ Copy to Device
```bash
scp build/bin/knulli-input-tester root@192.168.1.100:/tmp/
```

### 5️⃣ Install on Device
```bash
ssh root@192.168.1.100 << 'EOF'
cp /tmp/knulli-input-tester /usr/bin/
chmod +x /usr/bin/knulli-input-tester
mkdir -p /userdata/system/configs/input-tester
EOF
```

### 6️⃣ Run It!
```bash
ssh root@192.168.1.100 /usr/bin/knulli-input-tester
```

Press buttons on your controller. You should see events appear!

---

## Control Keys While Running

| Key | Action |
|-----|--------|
| **Fn + Y** | Cycle button schemes |
| **Fn + X** | Show/hide history |
| **Fn + Select** | Clear history |
| **Fn + Start** | Export as JSON |
| **Fn + Power** | Exit |

---

## Troubleshooting

### "SSH Connection Refused"
```bash
# SSH not enabled on Knulli
# Enable: System Settings → Network → SSH → ON
```

### "Binary crashes / No input events"
```bash
ssh root@192.168.1.100
# Check input devices
ls -la /dev/input/event*
```

### "Permission Denied"
```bash
ssh root@192.168.1.100
chmod 755 /usr/bin/knulli-input-tester
```

---

## Want to Run It on Startup?

```bash
ssh root@192.168.1.100 << 'EOF'
cat > /etc/systemd/system/knulli-input-tester.service << 'SERVICE'
[Unit]
Description=Knulli Input Tester
After=network.target

[Service]
Type=simple
User=root
ExecStart=/usr/bin/knulli-input-tester

[Install]
WantedBy=multi-user.target
SERVICE

systemctl daemon-reload
systemctl enable knulli-input-tester
systemctl start knulli-input-tester
EOF
```

View logs:
```bash
ssh root@192.168.1.100 journalctl -u knulli-input-tester -f
```

---

## Full Documentation

- **DEPLOYMENT.md** - Complete guide with all options
- **QUICKSTART.md** - Usage guide
- **README.md** - Project overview

---

## Need Help?

```bash
# Run with debug output
ssh root@192.168.1.100 /usr/bin/knulli-input-tester --log-level debug

# Check what input devices exist
ssh root@192.168.1.100 ls -la /dev/input/

# See all processes
ssh root@192.168.1.100 ps aux | grep knulli
```

**That's it! 🎮**
