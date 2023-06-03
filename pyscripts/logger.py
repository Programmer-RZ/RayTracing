def errorLog(message, command):
	print(f"<<ERROR>> FAILED TO EXECUTE COMMAND {command}")
	print(f"<<MESSAGE>> {message}")

def successLog(message, command):
	print(f"<<SUCCESS>> EXECUTED COMMAND {command}")
	print(f"<<MESSAGE>> {message}")