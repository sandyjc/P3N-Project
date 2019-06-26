import sqlite3
import datetime

POSITION_DATABASE = '__HOME__/project/positions.db'
COMMAND_DATABASE = '__HOME__/project/commands.db'
USER_DATABASE = '__HOME__/project/user.db'

#STATE should always be either "draw" or "stop" in order to indicate being done drawing a line

#USER will be the person who is drawing

#OWNER will be the user whose drawing User is drawing on

def create_position_database():
    conn = sqlite3.connect(POSITION_DATABASE)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS points_table (user text, owner text, user_location text, state text, timing timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close()

def create_command_database():
    conn = sqlite3.connect(COMMAND_DATABASE)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS commands_table (user text, owner text, command text, timing timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close()

def lookup_position_database(owner, time):
    conn = sqlite3.connect(POSITION_DATABASE)  # connect to that database
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    if time == "0":
        timer = datetime.datetime.now() - datetime.timedelta(seconds=3)
    else:
        timer = datetime.datetime.strptime(time,"%Y-%m-%d %H:%M:%S.%f") # create time for fifteen minutes ago!
    things = c.execute('''SELECT * FROM points_table WHERE timing > ? AND owner = ? ORDER BY timing ASC;''',(timer,owner)).fetchall()
    ret = {"owner":owner, "users": [], "points":[],"states":[],"time":0}
    for x in things:
        try:
            ret["users"].append(x[0])
            ret["points"].append(list(eval(x[2])))
            ret["states"].append(x[3])
            ret["time"] = x[4]
        except:
            ret["users"].append(x[0])
            ret["points"].append(x[2])
            ret["states"].append(x[3])
            ret["time"] = x[4]
    conn.commit()
    conn.close()
    return ret

def lookup_command_database(owner, time):
    conn = sqlite3.connect(COMMAND_DATABASE)  # connect to that database
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    if time == "0":
        timer = datetime.datetime.now() - datetime.timedelta(seconds=3)
    else:
        timer = datetime.datetime.strptime(time,"%Y-%m-%d %H:%M:%S.%f") # create time for fifteen minutes ago!    things = c.execute('''SELECT * FROM commands_table WHERE timing > ? AND owner = ? ORDER BY timing ASC;''',(timer, owner)).fetchall()
    things = c.execute('''SELECT * FROM commands_table WHERE timing > ? AND owner = ? ORDER BY timing ASC;''',(timer, owner)).fetchall()
    ret = {"owner":owner, "users": [] ,"commands":[],"time":0}
    for x in things:
        ret["users"].append(x[0])
        ret["commands"].append(x[2])
        ret["time"] = x[3]
    conn.commit()
    conn.close()
    return ret

def lookup_tutorial_position_database(user, owner, time):
    conn = sqlite3.connect(POSITION_DATABASE)  # connect to that database
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    if time == "0":
        timer = datetime.datetime.now() - datetime.timedelta(seconds=3)
    else:
        timer = datetime.datetime.strptime(time,"%Y-%m-%d %H:%M:%S.%f") # create time for fifteen minutes ago!
    things = c.execute('''SELECT * FROM points_table WHERE timing > ? AND user = ? AND owner = ? ORDER BY timing ASC;''',(timer,user,owner)).fetchall()
    ret = {"user":user,"points":[],"states":[],"time":0}
    for x in things:
        try:
            ret["points"].append(list(eval(x[2])))
            ret["states"].append(x[3])
            ret["time"] = x[4]
        except:
            ret["points"].append(x[2])
            ret["states"].append(x[3])
            ret["time"] = x[4]
    conn.commit()
    conn.close()
    return ret

def lookup_tutorial_command_database(user, owner, time):
    conn = sqlite3.connect(COMMAND_DATABASE)  # connect to that database
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    if time == "0":
        timer = datetime.datetime.now() - datetime.timedelta(seconds=3)
    else:
        timer = datetime.datetime.strptime(time,"%Y-%m-%d %H:%M:%S.%f") # create time for fifteen minutes ago!
    things = c.execute('''SELECT * FROM commands_table WHERE timing > ? AND user = ? AND owner = ? ORDER BY timing ASC;''',(timer,user,owner)).fetchall()
    ret = {"user":user,"commands":[],"time":0}
    for x in things:
        ret["commands"].append(x[2])
        ret["time"] = x[3]
    conn.commit()
    conn.close()
    return ret

def get_entire_drawing(owner):
    conn = sqlite3.connect(POSITION_DATABASE)
    c = conn.cursor()
    things = c.execute('''SELECT * FROM points_table WHERE owner = ? ORDER BY timing ASC;''',(owner,)).fetchall()
    ret = {"owner":owner, "users":[], "drawing":[], "states":[], "times":[]}
    for x in things:
        try:
            ret['users'].append(x[0])
            ret['drawing'].append(list(eval(x[2])))
            ret['states'].append(x[3])
            ret['times'].append(datetime.datetime.strptime(x[4],"%Y-%m-%d %H:%M:%S.%f").timestamp()*1000)
        except:
            ret['users'].append(x[0])
            ret['drawing'].append(x[2])
            ret['states'].append(x[3])
            ret['times'].append(datetime.datetime.strptime(x[4],"%Y-%m-%d %H:%M:%S.%f").timestamp()*1000)
    conn.commit()
    conn.close()
    return ret

def get_entire_command(owner):
    conn = sqlite3.connect(COMMAND_DATABASE)
    c = conn.cursor()
    things = c.execute('''SELECT * FROM commands_table WHERE owner = ? ORDER BY timing ASC;''',(owner,)).fetchall()
    ret = {"owner":owner, "users":[], "commands":[], "times":[]}
    for x in things:
        ret['users'].append(x[0])
        ret['commands'].append(x[2])
        ret['times'].append(datetime.datetime.strptime(x[3],"%Y-%m-%d %H:%M:%S.%f").timestamp()*1000)
    conn.commit()
    conn.close()
    return ret

def insert_into_command_database(user, owner, command):
    conn = sqlite3.connect(COMMAND_DATABASE)
    c = conn.cursor()
    if command == "initialize":
        c.execute('''INSERT into commands_table VALUES (?,?,?,?);''',(user,owner,"change line width 1",datetime.datetime.now()))
        c.execute('''INSERT into commands_table VALUES (?,?,?,?);''',(user,owner,"change line color black",datetime.datetime.now()))
        conn.commit()
        conn.close()
        conn = sqlite3.connect(POSITION_DATABASE)
        c = conn.cursor()
        c.execute('''INSERT into points_table VALUES (?,?,?,?,?);''',(user,owner,"null","STOP",datetime.datetime.now()))
        conn.commit()
        conn.close()
    else:
        c.execute('''INSERT into commands_table VALUES (?,?,?,?);''',(user,owner,command,datetime.datetime.now()))
        conn.commit()
        conn.close()
    
def insert_into_position_database(user, owner, points, state):
    points = points.split(' ')
    if points[-1] == '':
        points = points[:-1]
    conn = sqlite3.connect(POSITION_DATABASE)
    c = conn.cursor()
    for point in points:
        if point == "null":
            c.execute('''INSERT into points_table VALUES (?,?,?,?,?);''',(user,owner,"null","STOP",datetime.datetime.now()))
        else:
            x,y,z = point.split(",")
            c.execute('''INSERT into points_table VALUES (?,?,?,?,?);''',(user,owner,'({},{},{})'.format(x,y,z),state,datetime.datetime.now()))
    conn.commit()
    conn.close()

def request_handler(request):
    if request['method'] == 'GET':
        if request['values']['type'] == "init":
            create_position_database()
            return get_entire_drawing(request['values']['owner'])
        if request['values']['type'] == "init command":
            create_command_database()
            return get_entire_command(request['values']['owner'])
        if request['values']['type'] == "position":
            create_position_database()
            return lookup_position_database(request['values']['owner'], request['values']['time'])
        if request['values']['type'] == "command":
            create_command_database()
            return lookup_command_database(request['values']['owner'], request['values']['time'])
        if request['values']['type'] == "tutorial position":
            create_position_database()
            return lookup_tutorial_position_database(request['values']['user'], request['values']['owner'], request['values']['time'])
        if request['values']['type'] == "tutorial command":
            create_command_database()
            return lookup_tutorial_command_database(request['values']['user'], request['values']['owner'], request['values']['time'])
        return "Unsupported get request"
    if request['method'] == 'POST':
        if request['form']['type'] == "position":
            create_position_database()
            insert_into_position_database(request['form']['user'], request['form']['owner'], request['form']['points'], request['form']['state'])
            return {"user":request['form']['user'],"state":request['form']['state'],"points":request['form']['points']}
        if request['form']['type'] == "command":
            create_command_database()
            insert_into_command_database(request['form']['user'], request['form']['owner'], request['form']['command'][1:-1].replace(".","").replace("?","").replace("!","").lower())
            return "The command \"" + request["form"]["command"] + "\" was successful!"
    return "Unsupported post request"