#!/bin/bash
cd Desktop/Fingerprint_backend
cd
echo "Starting Fingerprint Server"
sleep 20s
cd Desktop/Fingerprint
source env/bin/activate
cd
cd Desktop/Fingerprint_using_mysql/FINGERPRINT_MODIFIED
echo "Starting Streamlit Application"
streamlit run Home.py --server.headless True