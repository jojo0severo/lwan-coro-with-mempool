from loop import Loop
import random


def callback(x):
    x *= x

    return {"a": random.random()}


class Batata:
    def __init__(self):
        self.a = 2


arg = Batata()

l = Loop()
a = l.create_coro(callback, arg)
b = l.create_coro(callback, arg)

print(l.run([a, b]))

print(arg.a)
