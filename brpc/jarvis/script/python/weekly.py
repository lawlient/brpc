#! /usr/bin/python3
#! -*- encoding:utf-8 -*-

import requests
import json
import datetime
import calendar
import os
import matplotlib.ticker as ticker
import matplotlib.gridspec as gridspec
import matplotlib.pyplot as plt

import smtplib
from email.mime.text import MIMEText
from email.header import Header
from email.mime.base import MIMEBase
from email.mime.multipart import MIMEMultipart
from email.mime.image import MIMEImage

DAILYPNG="./day.png"
TOTALPNG="./total.png"
 

# 查询date=d，classify分类的时间并追加至y数组中
def day(d, y, classify):
    url = "http://localhost/jarvis/Jarvis/GetTask?status=1&date="+d
    response = requests.get(url)
    if (response.status_code != 200):
        return 
    if ()
    body = json.loads(response.text)
    tasks = body["data"]["items"]
    i = 0
    for t in tasks:
        if (t["classify"] == classify):
            i += t["acthour"]
    y.append(i)
    return

# 时间字符串往后一天
def nextday(d):
    tmp = datetime.datetime.strptime(d, "%Y-%m-%d")
    tmp = tmp + datetime.timedelta(days=1)
    d   = tmp.strftime("%Y-%m-%d")
    return d


# 将生成的图片以邮件发送至邮箱
def send2me():
    smtpserver="smtp.163.com"
    username = "jiuanx2011@163.com"
    passwd = "MWOZZXPTRSQUIYVS"

    # 创建MIMEMultipart对象，表示邮件的整体
    msg=MIMEMultipart('related')  

    msg['Subject']='Monthly Report'
    msg['From']='jiuanx2011@163.com'
    msg['To']='jiuanx2011@163.com'

    # 创建邮件正文
    body=MIMEText('<html><body> '
                  'This is a monthly report from Jarvis.'
                  ' <br> '
                  '<img src="cid:cid1" alt="Time">'
                  '<img src="cid:cid2" alt="Total">'
                  '</body></html>', 'html', 'utf-8') 
    msg.attach(body)

    with open(DAILYPNG, "rb") as fp:
        img=MIMEImage(fp.read())
    
    img.add_header("Content-ID", "cid1")
    msg.attach(img)

    with open(TOTALPNG, "rb") as fp2:
        img2=MIMEImage(fp2.read())
    
    img2.add_header("Content-ID", "cid2")
    msg.attach(img2)

    server=smtplib.SMTP_SSL(smtpserver, port=465)
    server.login(username, passwd)
    server.sendmail(msg['From'], msg['To'], msg.as_string())
    server
    print("Email send successfully.")
    

 
x=[]
i=[]
t=[]
b=[]
s=[]
r=[]
o=[]
now = datetime.datetime.now()
d=datetime.datetime(now.year, now.month, 1)
d=d.strftime('%Y-%m-%d')
for k in range(0,31):
    x.append(d)
    day(d, y=i, classify=0)
    day(d, y=t, classify=1)
    day(d, y=b, classify=2)
    day(d, y=s, classify=3)
    day(d, y=r, classify=4)
    lostcount = (24 - i[k] - t[k] - b[k] - s[k] - r[k])
    o.append(lostcount)
    d = nextday(d)

# 绘制折线图
fig, ax = plt.subplots(figsize = (16,9), dpi = 80)

# ax.spines["left"].set_visible(False)
# ax.spines["top"].set_visible(False)
# ax.spines["right"].set_visible(False)


ax.plot(x, i, t, b, s, r, o, marker="o", mfc="white", ms=5)
 
ax.xaxis.set_major_locator(ticker.MultipleLocator(2))

ax.grid(ls = "--", lw = 0.5, color = "#4E616C")

ax.xaxis.set_tick_params(length = 2, color = "#4E616C", labelcolor = "#4E616C", labelsize = 6)
ax.yaxis.set_tick_params(length = 2, color = "#4E616C", labelcolor = "#4E616C", labelsize = 6)
 
ax.spines["bottom"].set_edgecolor("#4E616C")

# 添加标题和坐标轴标签
plt.title('Time')
plt.xlabel('day')
plt.xticks(rotation=270)
plt.ylabel('hour')
 
# 显示图形
plt.show()
plt.savefig(DAILYPNG)


plt.close()


c=['I', 'T', 'B', 'S', 'R', 'O']

ct=[]
ct.append(sum(i))
ct.append(sum(t))
ct.append(sum(b))
ct.append(sum(s))
ct.append(sum(r))
ct.append(sum(o))

# 饼图属性设置
colors = ['#FF9999', '#FFE888', '#FFCC99', '#CCFF99', '#99FF99', '#CC99FF']

# 绘制饼图
plt.pie(ct, labels=c, colors=colors, autopct='%1.1f%%', startangle=90)
 
# 设置饼图为圆形（非椭圆）
plt.axis('equal')

plt.savefig(TOTALPNG)

plt.close()

send2me()

os.remove(DAILYPNG)
os.remove(TOTALPNG)
