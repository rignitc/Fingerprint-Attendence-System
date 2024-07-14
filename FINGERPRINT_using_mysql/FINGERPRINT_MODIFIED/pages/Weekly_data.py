import streamlit as st
import pandas as pd
import mysql.connector
import warnings
warnings.filterwarnings("ignore")
import streamlit_authenticator as stauth
import yaml
from yaml.loader import SafeLoader
with open('../config.yaml') as file:
    config = yaml.load(file, Loader=SafeLoader)
st.set_page_config(page_title="RIG NITC",page_icon=":calendar:",layout="wide")
st.title(":ledger:  WEEKLY ATTENDENCE REPORT")
st.markdown('<style>div.block-container{padding-top:2rem;}</style>',unsafe_allow_html=True)
background_image = """
<style>
[data-testid="stAppViewContainer"] > .main {
    background-image: url("https://img.freepik.com/free-photo/glowing-molecular-structure-futuristic-galaxy-backdrop-generated-by-ai_188544-27827.jpg");
    background-size: cover;  /* This sets the size to cover the entire area */
    background-position: center;  
    background-repeat: no-repeat;
}
</style>
"""

st.markdown(background_image, unsafe_allow_html=True)
conn = mysql.connector.connect(user='root', password='root',host='localhost',database='rig_database')
cur = conn.cursor()
df =pd.read_sql("SELECT * FROM weekly_data",conn)

authenticator = stauth.Authenticate(
    config['credentials'],
    config['cookie']['name'],
    config['cookie']['key'],
    config['cookie']['expiry_days'],
    config['pre-authorized']
)
authenticator.login(location='main',max_login_attempts=5,max_concurrent_users=50,)
with open('../config.yaml', 'w') as file:
    yaml.dump(config, file, default_flow_style=False)
if st.session_state["authentication_status"]:
    st.write(df.style.background_gradient(cmap='Blues'))
    st.button(label="Logout", on_click=authenticator.logout)
else:st.write("CONTACT ADMIN FOR CREDENTIALS")
