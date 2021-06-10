import urllib.request

url = "http://192.168.0.200:8000/&V22=?"

response = urllib.request.urlopen(url)
html = response.read()
print(html)