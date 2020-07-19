
cls

.( Demo 'create', 'does>', 'here', and , ) cr cr

\  0        1         2         3         4         5         6         7         8
\  12345678901234567890123456789012345678901234567890123456789012345678901234567890
.( The 'create' - 'does>' pair is the hardest part of forth language. Sir yap      )
.( taught me this way " forth 'create' - 'does>' is simply to define an OO class   )
.( with any number of properties but with only one method."                        ) cr

.( Consider forth was appeared around the same time as C language .. or earlier.   ) 
.( So early that Object Oriented concept was too aggressive to most people.        )
.( However, we can understand it now because we already have some knowledge of     )
.( Object Oriented programming.                                                    ) cr

.( The 'constant' command word is actually created by 'create' - 'does>' pair, its )
.( definition is,                                                                  ) cr
.(      : constant create ,  does>  @  ; 										   ) cr

.( The new word CONSTANT itself is a OO class. How to use it?                      )

.( 123 constant one-two-three )
123 constant one-two-three             \ now one-two-three is a new word created by the 'create' command in 'constant'
                                       \ it's value is given by the ',' command in 'constant' that compiles the TOS which was 123 into
                                       \ one-two-three's PFA. So this is the class constant's property for one-two-three.
                                       \ When one-two-three executed, actually executes the '@' command defined by does>.
                                       \ So does> defines the method of the 'constant' class. In this case the method is
                                       \ to fetch '@' the PFA of one-two-three. That's 123.
                                       
cr one-two-three . cr

bye
