	
.global track_score
.global track_coins
track_score:	
	cmp r0, #0		@ make sure player has not touched a portal
	beq update_score	@ if the player has not, update their points
	bne zero_score		@ if they have make their score zero
.update_score:
	add r1, #1		@ add 1 to the points
.zero_score:
	mov r1, #0		@ make the players score 0
.done:
	mov r0, r1		@ move the points into r0 for return
	mov pc, lr		@ return

track_coins:	
	cmp r0, #1 		@ check if player has collected a coin
	beq update_coins	@ update the coin score if they have
	bne done		@ if they have not then return
.update_coins:
	add r1, #1		@ add 1 to the amount of coins
.done:
	mov r0, r1		@ move return value into r0
	mov pc, lr		@ return
	
