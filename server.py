from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/")
def hello():
    distance1 = int(request.args.get("distance1"))
    distance2 = int(request.args.get("distance2"))

    car_both = """
        Car:

        ____________________
    |                    || !!!
    |                    || !!!
    |                    || !!!
    |____________________|| !!!
                        !!!!
                        !!!!
        """
    car_left = """
        Car:

        ____________________
    |                    ||
    |                    ||
    |                    ||
    |____________________||
                        !!!!
                        !!!!
        """
    car_back = """
        Car:

        ____________________
    |                    || !!!
    |                    || !!!
    |                    || !!!
    |____________________|| !!!
        """

    stop = """
    SSSSS  TTTTTTT  OOOOO  PPPPPP
    S         T     O     O P     P
    SSSSS    T     O     O PPPPPP
        S   T     O     O P
    SSSSSS    T      OOOOO  P
    """

    if distance1 <= 10 or distance2 <= 10:
        print(stop)
    else if distance1 <= 30 and distance2 <= 30:
        print(car_both)
    else if distance1 <= 30:
        print(car_back)
    else if distance2 <= 30:
        print(car_left)

