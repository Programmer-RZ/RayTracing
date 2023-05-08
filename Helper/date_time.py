from datetime import datetime

def getDateTime():
    current_time = datetime.now()

    month = str(current_time.month)
    day = str(current_time.day)
    hour = str(current_time.hour)
    minute = str(current_time.minute)
    second = str(current_time.second)

    if int(month) < 10:
        month = "0" + month

    if int(day) < 10:
        day = "0" + day
       
    if int(hour) < 10:
        hour = "0" + hour
        
    if int(minute) < 10:
        minute = "0" + minute
        
    if int(second) < 10:
        second = "0" + second
        

    date = f"{current_time.year}-{month}-{day}"
    time = f"{hour}-{minute}-{second}"
    return (date + "_" + time)