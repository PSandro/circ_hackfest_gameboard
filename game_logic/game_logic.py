from enum import Enum

class Player():
    def __init__(self):
        # number of money and resources for player
        self.money = 10
        self.resource = 7
        self.products = []
        self.current_field_id = -1 # maybe start with -1 if no player figure has been set on the board
        self.current_round = 0

class Field():
    def __init__(self, own_id, state, neighbor_ids):
        self.own_id = own_id
        self.state = state
        self.neighbor_ids = neighbor_ids

class FieldState(Enum):
    EMPTY=1
    WASTE=2
    PRODUCT=3
    BUILDING=4

# TODO building classes

class Product():
    def __init__(self, field_id):
        self.field_id = field_id
        self.age = 0

class Game():
    def __init__(self):
        self.fields = []
        self.players = []

        player1 = Player()
        player2 = Player()

        # TODO initiate and hard-code fields

    def game_tick(self):
        for player in self.players:
            self.current_round += 1

            # update player's attributes
            player.money += len(products)
            for product in products:
                product.age += 1

            # handle: does player move or do they stay
            # if move: update current_position_id; player can build at the new position
            # if stay: activate building
            new_product_list = []
            for product in products:
                if product.age > 3: # TODO verify
                    # find field by product.field_id and set status from PRODUCT to WASTE
                    pass
                else:
                    new_product_list.append(product)
            player.products = new_product_list



