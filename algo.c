//Algo made by Kumachan on May 5th, 2017
//Copyright © 2017 Kumachan. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define mili 1200
typedef struct{
    int number;//カードの数字を表す　0から11まで
    int color;// カードの色を表す　黒なら0, 赤なら1
    int open_or_close;//カードが表か裏かを表す 表なら1, 裏なら0
    int player_or_com;//カードがプレイヤーのカードなら1,コンピュータのカードなら0
}card;
typedef struct{
    int left_open;
    int right_open;
}LR;

int appeared[12][2];//対象のカードか既に使われていたら1, 未使用なら0
int attacked[20][12][2];//その場所のその数字,色のカードをアタックしていたら1,そうでなければ0

int stay(int);
void game_start(void);
int decide_dealer(void);
void red_black(card);
card choose_card(int);
void cards_print(card[],int,int);
void point_calculator(int*,int*,int,int);
void shift(card[],card, int);
void actof_player(card[], card[], int*, int*, int*, int*);
void actof_com(card[], card[], int*, int*, int*, int*);
LR com_attack(card[],int);
int judgement(card[], card[], card, int, int,int);
int com_number(card[],card[], card, int,LR);
int com_conti(card[],int);
int game_end(card[],card[],int,int);
void round_point(card[],card[],int,int, int*,int*,int);

int main(){
    card player[20],com[20];
    int player_point=400, com_point=400;
    int numofcards_player=4,numofcards_com=4;
    int dealer=-1;
    int judge;
    int turn=1, round=1, game=-1;
    
    srand((unsigned)time(NULL));
    while(game==-1||game==0){
        while(round<3){
            int i,j;
            if(round==1){
                if(game==-1) game_start();
                dealer=decide_dealer();
            }else{
                dealer=(dealer+1)%2;
                choose_card(0);
                if(dealer==1) printf("\n今度の先攻はあなたです\n");
                else printf("\n今度の先攻はコンピュータです\n");
            }
            
            for(i=0;i<20;i++){
                for(j=0;j<12;j++){
                    attacked[i][j][0]=0;
                    attacked[i][j][1]=0;
                }
            }
            
            for(i=0;i<4;i++){
                player[i]=choose_card(-1);
                player[i].open_or_close=0;
                player[i].player_or_com=1;
                com[i]=choose_card(-1);
                com[i].open_or_close=0;
                com[i].player_or_com=0;
            }
            cards_print(com, numofcards_com,com_point);
            cards_print(player, numofcards_player,player_point);
            
            while(1){
                printf("\nRound %d\n<Turn %d>\n",round,turn);
                if(dealer==1){
                    actof_player(player, com, &numofcards_player, &numofcards_com, &player_point, &com_point);
                    judge=game_end(player, com, numofcards_player, numofcards_com);
                    if(judge!=-1) break;
                    
                    actof_com(player, com, &numofcards_player, &numofcards_com, &player_point, &com_point);
                    judge=game_end(player, com, numofcards_player, numofcards_com);
                    if(judge!=-1) break;
                    if(turn==8){
                        printf("全てのカードが引かれました\n引き分けとしてラウンドを終了します\n");
                        break;
                    }
                }else{
                    actof_com(player, com, &numofcards_player, &numofcards_com, &player_point, &com_point);
                    judge=game_end(player, com, numofcards_player, numofcards_com);
                    if(judge!=-1) break;
                    
                    actof_player(player, com, &numofcards_player, &numofcards_com, &player_point, &com_point);
                    judge=game_end(player, com, numofcards_player, numofcards_com);
                    if(judge!=-1) break;
                    if(turn==8){
                        printf("全てのカードが引かれました\n引き分けとしてラウンドを終了します\n");
                        break;
                    }
                }
                turn++;
            }
            round_point(player, com, numofcards_player, numofcards_com, &player_point, &com_point, judge);
            stay(mili);
            if(turn<8) printf("このラウンドは");
            if(judge==1)printf("あなたの勝ちです\n");
            else if(judge==0){
                printf("コンピュータの勝ちです. コンピュータのカードはこうでした\n");
                for(i=0;i<numofcards_com;i++) com[i].open_or_close=1;
                cards_print(com, numofcards_com, -1);
            }
            turn=1; round++;
            printf("Com:%03d\nYou:%03d\n",com_point,player_point);
            numofcards_player=4;
            numofcards_com=4;
        }
        if(player_point>com_point) printf("あなたの勝ちです\n");
        else if(player_point<com_point) printf("コンピュータの勝ちです\n");
        else printf("引き分けです\n");
        round=1;
        printf("点数を引き継いで, もう１ゲームやる場合は　0 を入力してEnterキーを押してください\n点数をリセットして, もう１ゲームやる場合は　-1 を入力してEnterキーを押してください\n終了する場合はそれ以外の数を入力して, Enterキーを押してください\n");
        scanf("%d",&game);
        if(game==-1){
            player_point=400;
            com_point=400;
        }
    }
    return 0;
    
}

int stay(int milsec){
    clock_t start, end;
    start=clock();
    do{
        if((end=clock())==(clock_t)-1) return 0;
    }while(1000*(double)(end-start)/CLOCKS_PER_SEC<milsec);
    return 1;
}

void game_start(void){
    int rule;
    
    printf("数当てゲーム algo へようこそ！\nルールを見ますか？\n(0)はい\t(1)いいえ\t");
    scanf("%d",&rule);
    while(rule!=0&&rule!=1){
        printf("もう一度入力してください\nルールを見ますか？\n(0)はい\t(1)いいえ\t");
        scanf("%d",&rule);
    }
    while(rule==0){
        printf("\nこのゲームでは0~11までの整数がそれぞれ書かれた赤のカードと黒のカードの計24枚のカードを使います. 初期ポイントは 400 で最終的にポイントが多い方の勝利です.\n推理力を生かして, コンピュータに勝利しましょう.\n\n");
        stay(2*mili);
        printf("カードには大きさがあり, 数字が異なるカードは数字が大きいカードが「大きいカード」数字が同じ場合は黒のカードより赤のカードの方が「大きいカード」とします.\n小さい　　");
        int i,j;
        card cards,tempcard;
        tempcard.color=-1;
        for(i=0;i<12;i++){
            for(j=0;j<2;j++){
                cards.open_or_close=0,cards.player_or_com=1;
                cards.number=i, cards.color=j;
                red_black(cards);
                red_black(tempcard);
            }
        }
        printf("　　大きい");
        printf("\n\n");
        stay(2*mili);
        printf("(1)親を決めます. カードを引き, 大きいカードを引いた方が親です.\n\n");
        stay(2*mili);
        printf("(2)始めに4枚のカードがそれぞれ配られ, 大きいカードほど右に配置されます. 相手のカードの数字は裏になっていて見ることができませんが色は常に知ることができます.\n\n");
        stay(2*mili);
        printf("(3)まず親がカードを1枚引きます. 引いてきたカードを相手に後悔しないまま, 相手のカードの中から当てるカードを決めます. そして, 決めたカードの数字を推定し, 入力します(これをアタックといいます).\n\n");
        stay(2*mili);
        printf("(4)アタックされた相手は, 数字が合っていたら「Yes!」、まちがっていたら「No!」と言います。\n\n");
        stay(2*mili);
        printf("(5-1)「No!」と言われたら, 親はアタックに使ったカードを表にして自分の列に並べます(このときに追加する位置も大きいカードほど右に来るようにします). 相手のカードはそのままで, アタックを交代します.\n\n");
        stay(2*mili);
        printf("(5-2)「Yes!」の場合, 相手は当てられたカードをオープンします. カードを当てると10ptsが,その当てたカードの数字が 6 ならさらに10ptsを相手から得ることができます. その後, アタックを続けるか, ステイするかを選択することができます.\nアタックを続ける場合, 今引いてきたカードを伏せたまま, はずれるまで、何度でもアタックできますが, 「No!」と言われたら, そのカードはオープンして自分の列に並べます.\nステイする場合, アタックをやめて, 今のカードをふせたまま自分の列に並べ, アタックを交代します. 何度かアタックした後にステイすることもできます.\n\n");
        stay(2*mili);
        printf("(6)どちらかが相手のカードを全て当てるか, 全てのカードが引かれたら, 1ラウンドが終了です. ラウンドの勝利者は相手から「残ったカードの枚数」×10点をもらえます. 残ったカードの中に 6 があるときは, ボーナス点として, 6 1枚につき, さらに10点もらえます.\n\n");
        stay(2*mili);
        printf("(7)次に親を交代して, もう1ラウンドやり, 2ラウンドで1ゲームが終了です.\n\n");
        stay(2*mili);
        printf("このプログラムでは・・・\n上がコンピュータで下があなたです\nあなたのカードのうち, 表になっているものは数字の横に * をつけて表示します.\nあなたが当てるべきカードの数字を ? で表します\n\n");
        stay(2*mili);
        int temp=1;
        while(temp!=0){
            printf("理解しましたよね?\n(0)はい\t");
            scanf("%d",&temp);
        }
        rule=-1;
    }
}//ゲームの開始時に実行. ようこそと表示、ルールを表示

int decide_dealer(void){
    int player_or_com;//プレイヤーが親なら1, コンピュータが親なら0
    int dammy;
    card player,com;
    card temp;
    temp.color=-1;
    choose_card(0);
    com=choose_card(-1);
    com.open_or_close=1;
    
    printf("\n親を決めます\nカードを引くには何か数字を入力してEnterキーを押してください\n");
    scanf("%d",&dammy);
    player=choose_card(-1);
    player.open_or_close=1;
    printf("あなたのカードは");
    red_black(player);
    red_black(temp);
    printf("でcomのカードは");
    red_black(com);
    red_black(temp);
    printf("なので先攻は");
    if(player.number==com.number){
        if(player.color>com.color){
            printf("あなたです\n");
            player_or_com=1;
        }
        else{
            printf("コンピュータです\n");
            player_or_com=0;
        }
    }
    else if(player.number>com.number){
        printf("あなたです\n");
        player_or_com=1;
    }
    else{
        printf("コンピュータです\n");
        player_or_com=0;
    }
    printf("\n");
    choose_card(0);
    stay(mili);
    return player_or_com;
}//親を決定して表示.プレイヤーが親なら1, コンピュータが親なら0を返す.

void red_black(card cards){
    if(cards.color==-1){
        printf("\x1b[49m");
        printf("\x1b[39m");//文字と背景をデフォルトに戻す
    }else if(cards.color==0){
        printf("\x1b[40m");
        printf("\x1b[37m");//黒いカード
        if(cards.open_or_close==1&&cards.player_or_com==1) printf("*%2d ",cards.number);
        else if(cards.open_or_close==1||cards.player_or_com==1) printf(" %2d ",cards.number);
        else printf("  ? ");
    }else if(cards.color==1){
        printf("\x1b[41m");
        printf("\x1b[37m");//赤いカード
        if(cards.open_or_close==1&&cards.player_or_com==1) printf("*%2d ",cards.number);
        else if(cards.open_or_close==1||cards.player_or_com==1) printf(" %2d ",cards.number);
        else printf("  ? ");
    }
}//カードの表示時に背景色と文字色を変更する関数

card choose_card(int start){
    card choice;
    
    if(start==0){
        int i;
        for(i=0;i<12;i++){
            appeared[i][0]=0;
            appeared[i][1]=0;
        }
        choice.number=-1,choice.color=-1;
    }//ゲーム開始時には初期化
    else{
        while(1){
            choice.number=rand()%12;
            choice.color=rand()%2;
            if(appeared[choice.number][choice.color]!=1){
                appeared[choice.number][choice.color]=1;
                break;
            }
        }//カードの色と数字を決定する。重複がないように出てきたカードはappearedに記録
    }
    return choice;
}//カードを選ぶ関数

int comp(const void *a, const void *b){
    card s_a=*(card*)a, s_b=*(card*)b;
    if(s_a.number!=s_b.number) return s_a.number-s_b.number;
    else return s_a.color-s_b.color;
}//カードの並べ替え方

void cards_print(card user[20], int k,int point){
    int i;
    card temp;
    temp.color=-1;
    qsort(user,k,sizeof(card),comp);//カードを並べ替える
    if(point!=-1) printf("Point:%d\t",point);
    for(i=0;i<k;i++){
        red_black(user[i]);
    }
    red_black(temp);
    printf("\n");
}//ユーザの引いたゲーム中のカードを表示する関数

void point_calculator(int *player_point,int *com_point, int attacker,int number){
    if(attacker==1){
        if(number==6){
            *player_point=*player_point+20;
            *com_point=*com_point-20;
        }else{
            *player_point=*player_point+10;
            *com_point=*com_point-10;
        }
    }else{
        if(number==6){
            *player_point=*player_point-20;
            *com_point=*com_point+20;
        }else{
            *player_point=*player_point-10;
            *com_point=*com_point+10;
        }
    }
}

void shift(card player[20], card taken, int numofcards_player){
    int i, mark;
    for(i=0;;i++){
        if(player[i].number==taken.number){
            mark=i;
            break;
        }
    }
    int j;
    for(i=numofcards_player-1;i>mark;i--){
        for(j=0;j<12;j++){
            attacked[i][j][0]=attacked[i-1][j][0];
            attacked[i][j][1]=attacked[i-1][j][1];
        }
    }
    for(j=0;j<12;j++){
        attacked[mark][j][0]=0;
        attacked[mark][j][1]=0;
    }
}

void actof_player(card player[20], card com[20], int *numofcards_player,int *numofcards_com,int *player_point, int *com_point){
    int attack=-1,number=-1, conti=0;
    int dammy;
    card taken;
    card temp;
    temp.color=-1;
    
    taken=choose_card(-1);
    taken.player_or_com=1;
    taken.open_or_close=0;
    
    printf("\nあなたの番です\n");
    printf("カードを引くには何か数字を入力してEnterキーを押してください\n");
    scanf("%d",&dammy);
    printf("今引いてきたカードは");
    red_black(taken);
    red_black(temp);
    printf("です\n");
    while(conti==0){
        printf("アタックするカードは左から何番目ですか?\n");
        scanf("%d",&attack);
        while(attack>*numofcards_com||attack<1||com[attack-1].open_or_close==1){
            printf("そのカードはアタックできません\nカードを選びなおしてください\nアタックするカードは左から何番目ですか?\n");
            scanf("%d",&attack);
        }
        printf("カードの数字を推定してください\n");
        scanf("%d",&number);
        
        if(number!=com[attack-1].number){
            printf("No!\n");
            taken.open_or_close=1;
            *numofcards_player=*numofcards_player+1;
            player[*numofcards_player-1]=taken;
            cards_print(com, *numofcards_com,*com_point);
            shift(player, taken, *numofcards_player);
            cards_print(player, *numofcards_player,*player_point);
            conti=-1;
        }else{
            printf("Yes!\n");
            com[attack-1].open_or_close=1;
            point_calculator(player_point, com_point, 1, number);
            cards_print(com, *numofcards_com,*com_point);
            cards_print(player, *numofcards_player,*player_point);
            printf("アタックを続けますか?\n続けるには 0 を, ステイするなら 0以外の数字 を入力してください\n");
            scanf("%d",&conti);
            if(game_end(player, com, *numofcards_player, *numofcards_com)!=-1){
                conti=-1;
                printf("全部のカードを当てました!\n");
            }
            if(conti!=0){
                taken.open_or_close=0;
                *numofcards_player=*numofcards_player+1;
                player[*numofcards_player-1]=taken;
                cards_print(com, *numofcards_com,*com_point);
                shift(player, taken, *numofcards_player);
                cards_print(player, *numofcards_player,*player_point);
            }
        }
    }
}

void actof_com(card player[20], card com[20], int *numofcards_player,int *numofcards_com,int *player_point, int *com_point){
    LR hint;
    int attack;
    int number=-1, conti=0;
    
    card taken;
    card temp;
    temp.color=-1;
    
    taken=choose_card(-1);
    taken.player_or_com=0;
    taken.open_or_close=0;
    
    printf("\nコンピュータの番です\n今引いてきたカードは");
    red_black(taken);
    red_black(temp);
    printf("です\n");
    stay(mili);
    while(conti==0){
        printf("アタックするカードは左から何番目ですか?\n");
        hint=com_attack(player, *numofcards_player);
        if(hint.left_open==-1&&hint.right_open!=-1) attack=hint.right_open;
        else attack=hint.left_open+2;
        stay(mili);
        printf("%d\n",attack);
        
        printf("カードの数字を推定してください\n");
        number=com_number(player, com, taken, *numofcards_player,hint);
        stay(mili);
        printf("%d\n",number);
        if(number!=player[attack-1].number){
            printf("No!\n");
            taken.open_or_close=1;
            *numofcards_com=*numofcards_com+1;
            com[*numofcards_com-1]=taken;
            cards_print(com, *numofcards_com,*com_point);
            cards_print(player, *numofcards_player,*player_point);
            conti=-1;
        }else{
            printf("Yes!\n");
            player[attack-1].open_or_close=1;
            point_calculator(player_point, com_point, 0, number);
            cards_print(com, *numofcards_com,*com_point);
            cards_print(player, *numofcards_player,*player_point);
            printf("アタックを続けますか?\n続けるには 0 を, ステイするなら 0以外の数字 を入力してください\n");
            conti=com_conti(player, *numofcards_player);
            stay(mili);
            printf("%d\n",conti);
            if(game_end(player, com, *numofcards_player, *numofcards_com)!=-1){
                conti=-1;
                printf("全部のカードを当てました!\n");
            }
            if(conti!=0){
                taken.open_or_close=0;
                *numofcards_com=*numofcards_com+1;
                com[*numofcards_com-1]=taken;
                cards_print(com, *numofcards_com,*com_point);
                cards_print(player, *numofcards_player,*player_point);
            }
        }
    }
}

LR com_attack(card player[20],int numofcards_player){
    LR hint;
    hint.left_open=-1,hint.right_open=-1;
    int i,temp=-1;
    for(i=0;i<numofcards_player;i++){
        if(player[i].open_or_close==1&&hint.left_open==-1){
            hint.left_open=i;
            temp=i;
        }
        else if(player[i].open_or_close==1&&hint.left_open!=-1){
            hint.right_open=i;
            if(hint.right_open-hint.left_open==1){
                hint.left_open=hint.right_open;
                hint.right_open=-1;
            }
            else break;
        }
    }
    if(hint.left_open==numofcards_player-1&&hint.right_open==-1){
        hint.left_open=-1;
        hint.right_open=temp;
    }
    return hint;
}//0番目から始まるオープンなカードの左端及び右端の番号を返す, 該当するものがなければ-1を返す

int judgement(card player[20],card com[20], card taken, int numofcard_com, int number,int attack){
    int i;
    for(i=0;i<numofcard_com;i++){
        if(player[attack].color==com[i].color&&number==com[i].number) return 0;
    }
    if(player[attack].color==taken.color&&number==taken.number) return 0;
    return 1;
}

int com_number(card  player[20],card com[20], card taken, int numofcard_com,LR hint){
    int i;
    if(hint.left_open==-1&&hint.right_open!=-1){
        for(i=player[hint.right_open].number;i>=0;i--){
            if((player[hint.right_open].color==0||player[hint.right_open].color==player[hint.right_open-1].color)&&i==player[hint.left_open].number){}
            else if(judgement(player, com, taken, numofcard_com, i, hint.right_open-1)==1&&attacked[hint.right_open-1][i][player[hint.right_open-1].color]==0){
                attacked[hint.right_open-1][i][player[hint.right_open-1].color]=1;
                return i;
            }
        }
    }else if(hint.left_open==-1){
        for(i=0;i<12;i++){
            if(judgement(player, com, taken, numofcard_com, i, hint.left_open+1)==1&&attacked[0][i][player[hint.left_open+1].color]==0){
                attacked[0][i][player[hint.left_open+1].color]=1;
                return i;
            }
        }
    }else if(hint.right_open==-1){
        for(i=player[hint.left_open].number;i<12;i++){
            if((player[hint.left_open].color==1||player[hint.left_open].color==player[hint.left_open+1].color)&&i==player[hint.left_open].number){}
            else if(judgement(player, com, taken, numofcard_com, i, hint.left_open+1)==1&&attacked[hint.left_open+1][i][player[hint.left_open+1].color]==0){
                attacked[hint.left_open+1][i][player[hint.left_open+1].color]=1;
                return i;
            }
        }
    }
    else{
        for(i=player[hint.left_open].number;i<=player[hint.right_open].number;i++){
            if((player[hint.left_open].color==1||player[hint.left_open].color==player[hint.left_open+1].color)&&i==player[hint.left_open].number){}
            else if(judgement(player, com, taken, numofcard_com, i, hint.left_open+1)==1&&attacked[hint.left_open+1][i][player[hint.left_open+1].color]==0){
                attacked[hint.left_open+1][i][player[hint.left_open+1].color]=1;
                return i;
            }
        }
    }
    return 0;
}

int com_conti(card player[20],int numofcards_player){
    int i, conti=-1;
    for(i=0;i<numofcards_player-2;i++){
        if(player[i].open_or_close==1&&player[i+2].open_or_close==1){
            conti=0;
            break;
        }
    }
    return conti;
}

int game_end(card player[20], card com[20],int numofcards_player,int numofcards_com){
    int i,judge;
    judge=0;
    for(i=0;i<numofcards_player;i++){
        if(player[i].open_or_close==0) judge=-1;
    }
    if(judge==0) return 0;
    judge=1;
    for(i=0;i<numofcards_com;i++){
        if(com[i].open_or_close==0) judge=-1;
    }
    return judge;
}//コンピュータのカードが全て当たっていたら1, プレイヤーのカードが全て当たっていたら0,そうでなければ-1を返す

void round_point(card player[20],card com[20],int numofcards_player,int numofcards_com, int*player_point,int*com_point,int judge){
    int i;
    if(judge==1){
        for(i=0;i<numofcards_player;i++){
            if(player[i].open_or_close==0){
                if(player[i].number==6){
                    *player_point=*player_point+20;
                    *com_point=*com_point-20;
                }else{
                    *player_point=*player_point+10;
                    *com_point=*com_point-10;
                }
            }
        }
    }
    else if(judge==0){
        for(i=0;i<numofcards_com;i++){
            if(com[i].open_or_close==0){
                if(com[i].number==6){
                    *player_point=*player_point-20;
                    *com_point=*com_point+20;
                }else{
                    *player_point=*player_point-10;
                    *com_point=*com_point+10;
                }
            }
        }
    }
}
