
//&&=BOTH -- ||=Only One

if ((prev_var1<700 && current_var1>700) || (prev_var2>700 && current_var2<700) || (prev_var3>700 &&
current_var3<700) || (prev_var4>700 && current_var4<700)
|| (prev_var5<700 && current_var5>700)) //checking for transitions (BLACK TO WHITE FOR SENSOR 1&5) (WHITE TO BLACK for sensor 2,3,4)

{
    if(current_var1>700 && (current_var2>700 || current_var3>700))
    {
        //It detects PLUS NODE & Only Left Node
        Leftturn();
    }

    if(current_var1<700 && current_var5>700)
    {
        //TO CHECK ONLY RIGHT NODE
        Right_flag=1;

    }
    
    if(current_var1<700 || current_var2<700 ||current_var3<700 || current_var4<700 || current_var5<700)
    {
        //Dead END

        Uturn();
    }



}

    prev_var1=current_var1;
    prev_var2=current_var2;
    prev_var3=current_var3;
    prev_var4=current_var4;
    prev_var5=current_var5;


if(Right_flag)
    {
       if(current_var1<700 && current_var2<700 && current_var3<700 && current_var4<700 && current_var5<700)
       {
        //IF ONLY RIGHT NODE IS DETECT
        Rightturn();
        Right_flag=0
       
       }

       if(current_var1<700 && current_var2>700 && current_var3<700 && current_var4<700 && current_var5<700)
       {
        //IF STRAIGHT + RIGHT NODE DETECT THEN SIMPLY Right_flag becomes zero and bot dont take any turn.
        Right_flag=0
       
       }
       
    }

void Leftturn(){

    left_turn=1;

    while(left_turn){
    turn_flag1=1;

    if(turn_flag1){
    set_motor_speed(MOTOR_A_0,MOTOR_FORWARD,pwm);
    set_motor_speed(MOTOR_A_1,MOTOR_BACKWARD,pwm);
    //TURN TILL IT WILL DETECT WHITE LINE WHITE LINE DETECTED 2 3 4>700
    }

    if(current_var1<700 && current_var2>700 && current_var3>700 && current_var4>700 && current_var5<700)
    {
        turn_flag1=0;
        left_turn=0;
    }

    }
    
}

void Rightturn(){

    Right_turn=1;

    while(Right_turn){
    turn_flag2=1;

    if(left_flag2){
    set_motor_speed(MOTOR_A_0,MOTOR_BACKWARD,pwm);
    set_motor_speed(MOTOR_A_1,MOTOR_FORWARD,pwm);
    //TURN TILL IT WILL DETECT WHITE LINE WHITE LINE DETECTED 2 3 4>700
    }

    if(current_var1<700 && current_var2>700 && current_var3>700 && current_var4>700 && current_var5<700)
    {
        turn_flag2=0;
        right_turn=0;

    }

    }
    
}

void Uturn(){

    U_turn=1;

    while(U_turn){
    turn_flag3=1; 

    if(turn_flag3){
    set_motor_speed(MOTOR_A_0,MOTOR_FORWARD,pwm);
    set_motor_speed(MOTOR_A_1,MOTOR_BACKWARD,pwm);
    //TURN TILL IT WILL DETECT WHITE LINE WHITE LINE DETECTED 2 3 4>700
    }

    if(current_var1<700 && current_var2>700 && current_var3>700 || current_var4>700 || current_var5<700)
    {
        left_flag=0;
        left_turn=0;

    }

    }
    
}
