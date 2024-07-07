import streamlit as st
import pandas as pd
import mysql.connector
import streamlit_authenticator as stauth
import warnings
warnings.filterwarnings("ignore")
import yaml
from yaml.loader import SafeLoader

with open('config.yaml') as file:
    config = yaml.load(file, Loader=SafeLoader)
    
    
st.set_page_config(page_title="RIG NITC",page_icon=":robot_face:",layout="wide")
st.title(":ledger:  ATTENDENCE REPORT")
st.markdown('<style>div.block-container{padding-top:1rem;}</style>',unsafe_allow_html=True)

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





#Use thisto get data from csv 
    # df = pd.read_csv("data.csv",encoding = "ISO-8859-1")
#connecting data from mysql database
conn = mysql.connector.connect(user='root', password='root',host='localhost',database='rig_database')
cur = conn.cursor()
df =pd.read_sql("SELECT * FROM main_data",conn)





authenticator = stauth.Authenticate(
    config['credentials'],
    config['cookie']['name'],
    config['cookie']['key'],
    config['cookie']['expiry_days'],
    config['pre-authorized']
)
authenticator.login(location='main',max_login_attempts=5,max_concurrent_users=50,)
if 'status' not in st.session_state:st.session_state.status='0'
def handle_user():
		if st.session_state.status == '1':
			try:
				email_of_registered_user, username_of_registered_user, name_of_registered_user = authenticator.register_user(pre_authorization=False)
				if email_of_registered_user:
					st.success('User registered successfully')
			except Exception as e:
				st.error(e)
		elif st.session_state.status == '2':
			try:
				if authenticator.update_user_details(st.session_state["username"]):
					st.success('Entries updated successfully')
			except Exception as e:
				st.error(e)
handle_user()
# Update the yaml file whenever something is updated
with open('../config.yaml', 'w') as file:
    yaml.dump(config, file, default_flow_style=False)
if st.session_state["authentication_status"]:
        #FrontEnd for Sidebar
        st.sidebar.header(":traffic_light: Filter by")
        batch = st.sidebar.multiselect("Pick the Batch",df["Batch"].unique())
        weeknumber = st.sidebar.multiselect("Pick the Week Number",df["week_number"].unique())
        month = st.sidebar.multiselect("Pick the month",df["month_name"].unique())
        year = st.sidebar.multiselect("Pick the year",df["Year_number"].unique())
        hours_satisfied = st.sidebar.multiselect("Is WorkHours Satisfied ?",df["Work_hours_satisfied"].unique())
        

        # logic for filter
        selected = {
                'Batch': batch,
                'week_number': weeknumber,
                'month_name': month,
                'Work_hours_satisfied': hours_satisfied,
                'Year_number':year,
            }
        filtered_df=df.copy()
        for key,value in selected.items():
            if value:filtered_df=filtered_df[filtered_df[key].isin(value)]
            
        st.write(filtered_df.style.background_gradient(cmap='Blues'))
       
    
            
        col1,col2,col3 = st.columns([1,1,1])
        with col1:
            if st.button(label="Register"):st.session_state.status='1'
        with col2:
            if st.button(label="Update Your Credentials"):st.session_state.status='2'
        with col3:
            st.button(label="Logout", on_click=authenticator.logout)
		
        
else:
    
    st.write("CONTACT ADMIN FOR CREDENTIALS")

    




    
