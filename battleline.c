//Battleline made by Kumachan on June 10th,2017
//Copyright © 2017 Kumachan. All rights reserved.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define staysec 10

typedef struct{
    int number;
    int color;
    int open_or_close;//1ならopen, -1ならclose
    int betlay;//カードが裏切りの対象なら1,そうでなければ-1
}card;//色0は戦術カード, closeかつ色-1は表示をデフォルトに戻す, 色-2はデフォルトのうえで場にカードが置かれていないことを表す

typedef struct{
    int choose;
    int used_special;
}put_card_return;

typedef struct{
    int strength;
    int sum;
}ret_hand;

int appeared[70]={};//i(1<=i<=6)番目の色の数字j(1<=i<=10)のカードについて, appeared[(i-1)*10+(j-1)]をすでに引かれていれば1、山にあれば0とする, 戦術カードは60~69番目に当てる
char *special_cards[10]={"ア","ダ","援","盾","霧","泥","偵","転","脱","裏"};
char frag_get[4];
void chan();
int stay(int);
void clear_(card[],card[],int[],int[]);
int game_start(char*,char*);
void card_colors(card);
card choose_card(int,int*,int*);
int comp(const void*,const void*);
void print_cards(card[]);
void print_table(card[],card[],int[]);
void print_game(card[],card[],card[],card[],int[],int[],int,int);

int able_to_put(card,int,int,int,int[],int,int);
put_card_return put_card(int,card[],card[],int[],int[],int[],int*);
void take_card(card[],int, int*,int *);
int judges(card[],int,int);
ret_hand hand(card[],int,int,int);
int frag_getter(card[],card[],int, int,int);
int winner(int[]);

card alexander_darius(int);
card reinforce();
card shield();
void reconnaissance(card[],int,int*,int*);
void change(card[],int[],int[],int);
void escape(card[],int[],int frag[]);
void betrayal(card[],int[],card[],int[],int[],int);


int main(){
    char p1name[20],p2name[20];
    card player[7], player2[7];//手持ちのカード
    card player_table[36],player2_table[36];//i(1<=i<=9)番目のフラッグのj(1<=i<=4)番目のカードは,user_table[(i-1)*4+(j-1)]で書く
    int tablecards_player[9]={},tablecards_player2[9]={};//場の各フラッグのところにあるカードの枚数
    int turn_player[9]={},turn_player2[9]={}; //カードが各フラッグのところに揃ったターン数
    int spe_place[10];//10枚の戦術カードが1から始まる何番目のフラッグところにあるか. 山にあれば-1;
    int frags[9];//各フラッグがプレイヤー2のものになったら0, プレイヤーのものになったら1, 決まっていなければ2
    
    int spe_para=0;//戦術カードを使えるかどうかのパラメータ. プレイヤーが使ったら+1, プレイヤー2が使ったら-1する
    int leftofnum=60, leftspe=10;//山に残ったカードの枚数
    int dealer;
    int turn=1;
    int win=-10;
    
    put_card_return ret;
    int i;
    
    srand((unsigned)time(NULL));
    
    clear_(player_table, player2_table, frags, spe_place);
    dealer=game_start(p1name,p2name);
    if(dealer!=1){
        strcpy(frag_get,"12*");
        char temp[20];
        strcpy(temp,p1name);
        strcpy(p1name,p2name);
        strcpy(p2name,temp);
    }else{
        strcpy(frag_get,"21*");
    }
    for(i=0;i<7;i++){
        player[i]=choose_card(0,&leftofnum,&leftspe);
        player2[i]=choose_card(0,&leftofnum,&leftspe);
    }
    while(1){
        for(i=0;i<7;i++){
            player[i].open_or_close=1;
            player2[i].open_or_close=-1;
        }
        
        printf("Turn : %d\n",turn);
        printf("%sの番です\n",p1name);
        print_game(player, player2, player_table, player2_table, frags, spe_place, leftofnum, leftspe);
        
        ret=put_card(1, player, player_table, tablecards_player, spe_place, frags, &spe_para);
        switch (ret.used_special){
            case 6:
                reconnaissance(player,ret.choose,&leftofnum,&leftspe);
                break;
            case 7:
                change(player_table,tablecards_player,frags,spe_place[5]);
                break;
            case 8:
                escape(player2_table,tablecards_player2,frags);
                break;
            case 9:
                betrayal(player_table,tablecards_player,player2_table,tablecards_player2,frags,spe_place[5]);
                break;
        }
        
        take_card(player, ret.choose, &leftofnum, &leftspe);
        for(i=0;i<9;i++){
            if(spe_place[5]==i){
                if(tablecards_player[i]==4&&turn_player[i]==0) turn_player[i]=turn;
                else if(tablecards_player[i]<4) turn_player[i]=0;
            }
            else{
                if(tablecards_player[i]==3&&turn_player[i]==0) turn_player[i]=turn;
                else if(tablecards_player[i]<3) turn_player[i]=0;
            }
            
        }
        for(i=1;i<10;i++){
            if(turn_player[i-1]!=0&&turn_player2[i-1]!=0&&frags[i-1]==2){
                frags[i-1]=frag_getter(player_table, player2_table, i, spe_place[5], spe_place[4]);
                if(frags[i-1]==2){
                    if(turn_player[i-1]<turn_player2[i-1]) frags[i-1]=0;
                    else frags[i-1]=1;
                }
                if(frags[i-1]==0) printf("\n%sが%d番目のフラッグを獲得しました\n",p2name,i);
                else if(frags[i-1]==1) printf("\n%sが%d番目のフラッグを獲得しました\n",p1name,i);
            }
        }
        print_game(player, player2, player_table, player2_table, frags, spe_place, leftofnum, leftspe);
        win=winner(frags);
        
        if(win==-1){printf("\n%sの勝ちです\n",p2name); break;}
        else if(win==1){printf("\n%sの勝ちです\n",p1name); break;}
        stay(3000);
        chan();
        
        for(i=0;i<7;i++){
            player[i].open_or_close=-1;
            player2[i].open_or_close=1;
        }
        
        printf("%sの番です\n",p2name);
        for(i=0;i<7;i++){
            player[i].open_or_close=-1;
            player2[i].open_or_close=1;
        }
        print_game(player, player2, player_table, player2_table, frags, spe_place, leftofnum, leftspe);
        
        ret=put_card(0,player2, player2_table, tablecards_player2, spe_place, frags, &spe_para);
        switch (ret.used_special){
            case 6:
                reconnaissance(player2,ret.choose,&leftofnum,&leftspe);
                break;
            case 7:
                change(player2_table,tablecards_player2,frags,spe_place[5]);
                break;
            case 8:
                escape(player_table,tablecards_player,frags);
                break;
            case 9:
                betrayal(player2_table,tablecards_player2,player_table,tablecards_player,frags,spe_place[5]);
                break;
        }
        take_card(player2, ret.choose, &leftofnum, &leftspe);
        for(i=0;i<9;i++){
            if(spe_place[5]==i){
                if(tablecards_player2[i]==4&&turn_player2[i]==0) turn_player2[i]=turn;
                else if(tablecards_player2[i]<4) turn_player2[i]=0;
            }
            else{
                if(tablecards_player2[i]==3&&turn_player2[i]==0) turn_player2[i]=turn;
                else if(tablecards_player2[i]<3) turn_player2[i]=0;
            }
        }
        
        for(i=1;i<10;i++){
            if(turn_player[i-1]!=0&&turn_player2[i-1]!=0&&frags[i-1]==2){
                frags[i-1]=frag_getter(player_table, player2_table, i, spe_place[5], spe_place[4]);
                if(frags[i-1]==2){
                    if(turn_player[i-1]<turn_player2[i-1]) frags[i-1]=0;
                    else frags[i-1]=1;
                }
                if(frags[i-1]==0) printf("\n%sが%d番目のフラッグを獲得しました\n",p2name,i);
                else if(frags[i-1]==1) printf("\n%sが%d番目のフラッグを獲得しました\n",p1name,i);
            }
        }
        print_game(player, player2, player_table, player2_table, frags, spe_place, leftofnum, leftspe);
        win=winner(frags);
        if(win==-1){printf("\n%sの勝ちです\n",p2name); break;}
        else if(win==1){printf("\n¥%sの勝ちです\a\a\n",p1name); break;}
        turn++;
        stay(3000);
        chan();
    }
    return 0;
}

void chan(){
    int j;
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nプレイヤーを交代してください.\n始まるまで\n");
    for(j=staysec;j>0;j--){
        printf(" %2d\r",j);
        fflush(stdout);
        stay(1000);
    }
}

int stay(int milsec){
    clock_t start,end;
    start=clock();
    do{
        if((end=clock())==(clock_t)-1) return 0;
    }while(1000*(double)(end-start)/CLOCKS_PER_SEC<milsec);
    return 1;
}

void clear_(card player_table[36], card player2_table[36], int frag[9],int spe_place[10]){
    int i;
    
    for(i=0;i<36;i++){
        player_table[i].open_or_close=1;
        player_table[i].betlay=1;
        player_table[i].color=-2;
        player2_table[i].open_or_close=1;
        player2_table[i].betlay=1;
        player2_table[i].color=-2;
    }
    
    for(i=0;i<9;i++){
        spe_place[i]=-1;
        frag[i]=2;
    }
    spe_place[9]=-1;
}

int game_start(char p1name[20], char p2name[20]){
    int rule=-1;
    int dealer;
    FILE *rulefile;
    
    printf("ルールを見ますか?　「はい」を選択すると, ルールをbattleline_rule.txtという名前のファイルとして出力します.\n(0)はい　(1)いいえ\t");
    scanf("%d",&rule);
    while(rule!=0&&rule!=1){
        printf("入力が間違っています\nもう一度入力してください.\n(0)はい　(1)いいえ\t");
        scanf("%d",&rule);
    }
    if(rule==0){
        rulefile=fopen("battleline_rule.txt","w");
        fprintf(rulefile,"バトルラインでは, 兵士カードと呼ばれる1~10までの数字がそれぞれ書かれた6色のカード計60枚と戦術カードと呼ばれる特殊な能力を持つカード10枚を用います. 9つあるフラッグの5つ以上を獲得するか連続する3つのフラッグを獲得したプレイヤーの勝ちです.\n開始時に各プレイヤーにはそれぞれ7枚の兵士カードが手札として配られます.\n\n(1)まず先行となったプレイヤーは手札から1枚選び9つのフラッグのどれかの前におきます.\n\n(2)その後プレイヤーは兵士カードと戦術カードのいずれかを選んで1枚カードを引きます.\n\n(3)続いて, 後攻のプレイヤーも先攻と同様に(1),(2)の行動をします.\n\n(4)カードを1枚出して1枚引くこの操作を先行, 後攻と繰り返していきます. 各ターンで出すカードは兵士カード戦術カードどちらでも自由に選ぶことができます. 戦術カードの能力については後述します.\n\n(5)各フラッグの前にあるカードが3枚揃うと「役」が成立します. 同じフラッグのところの両者の「役」が確定した時点で, 強い役を作った方がそのフラッグを得ることができます. 役の強さが同じ場合には, カードの数字の和が大きい方が, それも同じ場合には早く役を完成させた方がフラッグを得ることができます.\n役の強さは以下の通りです:\n\n強\n3枚がすべて同色で, かつ連番\n3枚が同数, 色は問わない\n3枚がすべて同色, 数字は問わない\n3枚が連番, 色は問わない\n上記のどれにも当てはまらない\n弱\n(ただしここで連番には 9,10,1 や 10,1,2 は含みません)\n\n<戦術カードの能力>\nアレクサンダー, ダリウス\n好きな色・数字の兵士カードとして使うことができます. また, このカードは「裏切り」の対象にはなりません. 本プログラムでは「ア」「ダ」と表します.\n\n援軍騎兵\n好きな色の8の兵士カードとして使用できます. また, このカードは「裏切り」の対象にはなりません. 本プログラムでは「援」と表します.\n\n盾\n好きな色と1,2,3の中から好きな数字の兵士カードとして使用できます. また、このカードは「裏切り」の対象にはなりません.\n\n霧\n霧が配置されたフラッグはカードの合計値での勝負となります. 泥濘と同じ列に配置した場合, 両方が有効となります.\n\n泥濘\n通常は3枚での勝負であるのに対し, 泥濘が配置されたフラッグは, 4枚カードが必要になります. 霧と同じ列に配置した場合, 両方が有効となります. 本プログラムでは「泥」と表します.\n\n偵察\n兵士カード, 戦術カードの山札から好きな組み合わせで3枚までカードを引きます. その後, 手札から好きな組み合わせのカード(引いた枚数-1)枚を山札に戻します. 本プログラムでは「偵」と表します.\n\n配置転換\nまだ確保されていないフラッグに配置されている自分の任意の兵士カードに対して, まだ役が決定していない別のフラッグのところに移動するか, ゲームから除外することができます. 本プログラムでは「転」と表します.\n\n脱走\nまだ確保されていないフラッグに配置されている敵の任意の兵士カードをゲームから除外することができます. 本プログラムでは「脱」と表します.\n\n裏切り\nまだ確保されていないフラッグに配置されている敵の任意の兵士カードを奪って, まだ自分の役が決定していないフラッグのところに自分のカードとして再配置します. ただし「アレキサンダー」「ダリウス」「援軍騎兵」「盾」は対象となりません. 本プログラムでは「裏」と表します.\n\n\nあ, 言い忘れてましたが, 本プログラムでは, 下の手札, 場の左側が先攻, 上の手札, 場の右側が後攻の手札です. また, プレイヤー1がフラッグを獲得するとそのフラッグのマークが*から1に, プレイヤー2がフラッグを獲得するとそのフラッグのマークが*から2に変化します.\n\nでは, 友達を誘って遊んでみてください.\nえ？ ぼっちだから友達がいない？ 僕と同類ですね...\n\nくまちゃん");
        fclose(rulefile);
    }
    printf("プレイヤーの名前を入力してください(半角英数字で20文字未満)\n");
    printf("プレイヤー1:");
    scanf("%s",p1name);
    while(strlen(p1name)>19){
        printf("入力し直してください\n");
        printf("プレイヤー1:");
        scanf("%s",p1name);
    }
    srand((unsigned)time(NULL));
    printf("プレイヤー2:");
    scanf("%s",p2name);
    while(strlen(p2name)>19||strcmp(p1name,p2name)==0){
        printf("入力し直してください\n");
        printf("プレイヤー2:");
        scanf("%s",p2name);
    }
    
    printf("親を決めます\n何か数字を入力してEnterキーを押してください\n");
    scanf("%d",&dealer);
    dealer+=rand();
    dealer=dealer%2;
    if(dealer==0) printf("親は%sです\n\n",p2name);
    else printf("親は%sです\n\n",p1name);
    stay(1500);
    return dealer;
}

void card_colors(card a_card){
    if(a_card.open_or_close==-1){
        if(a_card.color==-1){
            printf("\x1b[49m");
            printf("\x1b[39m");//文字と背景をデフォルトに戻す
        }else{
            printf("\x1b[49m");
            printf("\x1b[39m");
            printf(" ?? ");
        }
    }else if(a_card.open_or_close==1){
        if(a_card.color==-2){
            printf("\x1b[49m");
            printf("\x1b[39m");
            printf("    ");//場でしか使わない
        }else if(a_card.color==0){
            printf("\x1b[49m");
            printf("\x1b[39m");
            printf(" %s ",special_cards[a_card.number]);
        }else if(a_card.color==1){
            printf("\x1b[41m");
            printf("\x1b[37m");
            printf(" %02d ", a_card.number);
        }else if(a_card.color==2){
            printf("\x1b[42m");
            printf("\x1b[37m");
            printf(" %02d ", a_card.number);
        }else if(a_card.color==3){
            printf("\x1b[43m");
            printf("\x1b[30m");
            printf(" %02d ", a_card.number);
        }else if(a_card.color==4){
            printf("\x1b[44m");
            printf("\x1b[37m");
            printf(" %02d ", a_card.number);
        }else if(a_card.color==5){
            printf("\x1b[45m");
            printf("\x1b[30m");
            printf(" %02d ", a_card.number);
        }else if(a_card.color==6){
            printf("\x1b[46m");
            printf("\x1b[30m");
            printf(" %02d ", a_card.number);
        }
    }
}

int comp(const void*a, const void*b){
    card s_a=*(card*)a,s_b=*(card*)b;
    if(s_a.color==s_b.color) return s_a.number-s_b.number;
    else return s_a.color-s_b.color;
}

void print_cards(card user[7]){
    int i;
    card temp;
    temp.color=-1,temp.open_or_close=-1;
    qsort(user, 7, sizeof(card), comp);
    for(i=0;i<7;i++) card_colors(user[i]);
    card_colors(temp);
    printf("\n");
}

void print_table(card player_table[36],card player2_table[36],int frag[9]){
    int i,j;
    card temp;
    temp.color=-1,temp.open_or_close=-1;
    for(i=0;i<9;i++){
        for(j=3;j>=0;j--) card_colors(player_table[i*4+j]);
        card_colors(temp);
        printf(" %c ",frag_get[frag[i]]);
        for(j=0;j<4;j++) card_colors(player2_table[i*4+j]);
        card_colors(temp);
        printf("\n");
    }
}

void print_game(card player[7],card player2[7],card player_table[36],card player2_table[36],int frag[9],int spe_place[10],int leftof_num,int leftof_spe){
    printf("兵士カード 残り：%d枚\t戦術カード 残り：%d枚\n",leftof_num,leftof_spe);
    print_cards(player2);
    printf("\n");
    print_table(player_table, player2_table, frag);
    printf("\n");
    print_cards(player);
    
    if(spe_place[4]==-1) printf("「霧」はまだ使われていません.\t");
    else printf("「霧」は%d番目のフラッグに使われています\t",spe_place[4]);
    if(spe_place[5]==-1) printf("「泥濘」はまだ使われていません.\n");
    else printf("「泥濘」は%d番目のフラッグに使われています\n",spe_place[5]);
    printf("\n");
}

card choose_card(int spe_or_not, int *leftof_num,int *leftof_spe){//spe_or_notは選ぶカードが戦術カードなら1, そうでなければ0
    card choice;
    if(spe_or_not!=1){
        while(1){
            choice.betlay=1;
            choice.color=rand()%6+1;
            choice.number=rand()%10+1;
            if(appeared[(choice.color-1)*10+choice.number-1]==0){
                appeared[(choice.color-1)*10+choice.number-1]=1;
                *leftof_num=*leftof_num-1;
                break;
            }
        }
    }
    else{
        while(1){
            choice.color=0;
            choice.number=rand()%10;
            if(appeared[60+choice.number]==0){
                appeared[60+choice.number]=1;
                *leftof_spe=*leftof_spe-1;
                break;
            }
        }
    }
    return choice;
}

int able_to_put(card put_card, int player_or_player2,int choose,int put,int tablecards_user[9],int quag,int spe_para){
    if(choose>7||choose<1||put>9||put<1) return 0;
    else if((put_card.color==0&&player_or_player2==1&&spe_para>=1)||(put_card.color==0&&player_or_player2==0&&spe_para<=-1)){
        printf("戦術カードはこれ以上使えません\n");
        return 0;
    }
    else if(quag!=put&&tablecards_user[put-1]>=3&&(put_card.color>0||(put_card.color==0&&put_card.number<4))) return 0;
    else if(quag==put&&tablecards_user[put-1]>=4&&(put_card.color>0||(put_card.color==0&&put_card.number<4))) return 0;
    else return 1;
}

put_card_return put_card(int player_or_player2, card user[7],card user_table[36],int tablecards_user[9],int spe_place[9],int frag[9],int*spe_para){
    put_card_return ret;
    ret.choose=-1,ret.used_special=-1;
    int put=-1;
    printf("左から何番目のカードを, 上から何番目のフラッグのところに置きますか？ スペース区切りで2つの数字を入力してください.\n(例えば, 左から3番目のカードを上から7番目のフラッグに置きたいときは\t3 7\tと入力してください)\nただし, 「偵察」「配置転換」「脱走」「裏切り」を使う際は,2つ目の数字はまだ所有者の決まっていないフラッグなら1~9番目のなんでも構いません.\n");
    scanf("%d%d",&ret.choose,&put);
    
    while(able_to_put(user[ret.choose-1], player_or_player2, ret.choose, put, tablecards_user, spe_place[5], *spe_para)!=1||frag[put-1]!=2){
        printf("そのカードは使えないかその場所には置けません. 入力しなおしてくだい.\n");
        scanf("%d%d",&ret.choose,&put);
    }
    if(user[ret.choose-1].color>0){
        tablecards_user[put-1]++;
        user_table[(put-1)*4+tablecards_user[put-1]-1]=user[ret.choose-1];
        user_table[(put-1)*4+tablecards_user[put-1]-1].open_or_close=1;
    }
    else if(user[ret.choose-1].color==0){
        if(player_or_player2==1) *spe_para=*spe_para+1;
        else *spe_para=*spe_para-1;
        spe_place[user[ret.choose-1].number]=put;
        
        switch (user[ret.choose-1].number){
            case 0:
                tablecards_user[put-1]++;
                user_table[(put-1)*4+tablecards_user[put-1]-1]=alexander_darius(0);
                break;
            case 1:
                tablecards_user[put-1]++;
                user_table[(put-1)*4+tablecards_user[put-1]-1]=alexander_darius(1);
                break;
            case 2:
                tablecards_user[put-1]++;
                user_table[(put-1)*4+tablecards_user[put-1]-1]=reinforce();
                break;
            case 3:
                tablecards_user[put-1]++;
                user_table[(put-1)*4+tablecards_user[put-1]-1]=shield();
                break;
            case 4:
                printf("戦術カード「霧」を使います\n");
                break;
            case 5:
                printf("戦術カード「泥濘」を使います\n");
                break;
            default:
                ret.used_special=user[ret.choose-1].number;
                break;
        }
    }
    return ret;
}

void take_card(card user[7],int choose, int *leftof_num,int *leftof_spe){
    int kind;
    
    if(*leftof_spe==0){
        printf("戦術カードは残っていません. 兵士カードを引きます\n");
        kind=-1;
    }else{
        printf("引くカードの種類はどちらにしますか?\n(0)兵士カード (1)戦術カード\t");
        scanf("%d",&kind);
        
        while(kind!=0&&kind!=1){
            printf("入力しなおしてください\n(0)兵士カード (1)戦術カード\t");
            scanf("%d",&kind);
        }
    }
    user[choose-1]=choose_card(kind,leftof_num,leftof_spe);
    
    user[choose-1].open_or_close=1;
}

int judges(card hand[4],int cards,int kind){//kindは判定番号. 1は連番, 2は同数, 3は同色
    int i;
    int result=1;
    if(kind==1){
        result=1;
        for(i=0;i<cards-1;i++){
            if(hand[i].number+1!=hand[i+1].number||hand[i].color==-2) result=0;
        }
        if(hand[cards-1].color==-2) result=0;
    }else if(kind==2){
        result=1;
        for(i=0;i<cards-1;i++){
            if(hand[i].number!=hand[i+1].number||hand[i].color==-2) result=0;
        }
        if(hand[cards-1].color==-2) result=0;
    }else if(kind==3){
        result=1;
        for(i=0;i<cards-1;i++){
            if(hand[i].color!=hand[i+1].color||hand[i].color==-2) result=0;
        }
        if(hand[cards-1].color==-2) result=0;
    }
    return result;
}

ret_hand hand(card user_table[36], int k,int quag, int smog){
    ret_hand ret;
    int cards;
    ret.sum=0;
    card hand[4];
    int i;
    if(k==quag) cards=4;
    else cards=3;
    
    for(i=0;i<cards;i++){
        hand[i]=user_table[(k-1)*4+i];
        ret.sum+=hand[i].number;
    }
    
    if(k==smog) ret.strength=ret.sum;
    qsort(hand,cards,sizeof(card),comp);
    
    if(judges(hand, cards, 1)==1&&judges(hand, cards, 3)==1) ret.strength=5;
    else if(judges(hand, cards, 2)==1) ret.strength= 4;
    else if(judges(hand, cards, 3)==1) ret.strength= 3;
    else if(judges(hand, cards, 1)==1) ret.strength= 2;
    else ret.strength= 1;
    return ret;
}

int frag_getter(card player_table[36],card player2_table[36],int k, int quag,int smog){
    if(hand(player_table, k, quag,smog).strength<hand(player2_table, k, quag, smog).strength) return 0;
    else if(hand(player_table, k, quag,smog).strength>hand(player2_table, k, quag, smog).strength) return 1;
    else if(hand(player_table, k, quag,smog).sum<hand(player2_table, k, quag, smog).sum) return 0;
    else if(hand(player_table, k, quag,smog).sum>hand(player2_table, k, quag, smog).sum) return 1;
    else return 2;
}

int winner(int frag[9]){
    int i;
    int player=0, player2=0;
    for(i=0;i<7;i++){
        if(frag[i]==0&&frag[i+1]==0&&frag[i+2]==0) return -1;//プレイヤー2の勝ち
        else if(frag[i]==1&&frag[i+1]==1&&frag[i+2]==1) return 1;//プレイヤーの勝ち
    }
    for(i=0;i<9;i++){
        if(frag[i]==0) player2++;
        else if(frag[i]==1) player++;
    }
    if(player2>=5) return -1;
    else if(player>=5) return 1;
    else return 0;
}

//以下, 戦術カードの機能を列挙.　泥濘と霧については上に随時書く
//"ア","ダ","援","盾","霧","泥","偵","転","脱","裏"

card alexander_darius(int ale_or_dari){//ale_or_dariが0ならアレクサンダー, 1ならダリウスとして使用
    card ale_dari;
    ale_dari.open_or_close=1;
    ale_dari.betlay=-1;
    if(ale_or_dari==0) printf("戦術カード「アレクサンダー」を使います\n");
    else if(ale_or_dari==1) printf("戦術カード「ダリウス」を使います\n");
    printf("色と数字をスペース区切りの2つの数字として指定してください. ただし, 色は次の選択群から選んでください：\n");
    printf("(1)赤 (2)緑 (3)黄色 (4)青 (5)マゼンダ (6)シアン\n");
    printf("(例えば,　青の7にしたければ 4 7 と入力してください)\n");
    
    scanf("%d%d",&ale_dari.color,&ale_dari.number);
    
    while ((ale_dari.color>6&&ale_dari.color<1)||(ale_dari.number<1&&ale_dari.number>10)){
        printf("入力しなおしてください\n");
        scanf("%d%d",&ale_dari.color,&ale_dari.number);
    }
    return ale_dari;
}

card reinforce(){
    card rein;
    rein.open_or_close=1;
    rein.betlay=-1;
    rein.number=8;
    printf("戦術カード「援軍騎兵」を使います\n");
    printf("色を次の選択群から選んでください：\n");
    printf("(1)赤 (2)緑 (3)黄色 (4)青 (5)マゼンダ (6)シアン\n");
    
    scanf("%d",&rein.color);
    
    while (rein.color>6||rein.color<1){
        printf("入力しなおしてください\n");
        scanf("%d",&rein.color);
    }
    return rein;
}

card shield(){
    card shie;
    shie.open_or_close=1;
    shie.betlay=-1;
    printf("戦術カード「盾」を使います\n");
    printf("色と数字1~3のをスペース区切りの2つの数字として指定してください. ただし, 色は次の選択群から選んでください：\n");
    printf("(1)赤 (2)緑 (3)黄色 (4)青 (5)マゼンダ (6)シアン\n");
    printf("(例えば,　緑の1にしたければ 2 1 と入力してください)\n");
    scanf("%d%d",&shie.color,&shie.number);
    
    while ((shie.color>6||shie.color<1)||(shie.number<1||shie.number>3)){
        printf("入力しなおしてください\n");
        scanf("%d%d",&shie.color,&shie.number);
    }
    return shie;
}

void reconnaissance(card user[7], int place,int *leftof_num,int *leftof_spe){
    int change;
    int leave[3]; card take[3];
    int i;
    
    leave[0]=-1,leave[1]=-2,leave[2]=-3;
    printf("戦術カード「偵察」を使います.\n");
    for(i=0;i<3;i++) { take[i].open_or_close=1; take[i].betlay=1;}
    printf("何枚のカードを交換しますか？(1枚〜３枚)\n");
    scanf("%d",&change);
    
    while(change<1||change>3){
        printf("入力しなおしてください\n");
        scanf("%d",&change);
    }
    printf("欲しいカードの色と数字をスペース区切りの2つの数字として指定してください. ただし, 色は次の選択群から選んでください：\n");
    printf("(1)赤 (2)緑 (3)黄色 (4)青 (5)マゼンダ (6)シアン\n");
    printf("(例えば,　青の7にしたければ 4 7 と入力してください)\n");
    
    for(i=0;i<change;i++){
        printf("%d枚目：",i+1);
        scanf("%d%d",&take[i].color,&take[i].number);
        
        while (appeared[(take[i].color-1)*10+take[i].number-1]!=0){
            printf("入力しなおしてください\n%d枚目：",i+1);
            scanf("%d%d",&take[i].color,&take[i].number);
        }
        *leftof_num=*leftof_num-1;
        appeared[(take[i].color-1)*10+take[i].number-1]=1;
    }
    
    printf("山に戻すカードを%d枚選んでください\n戻すカードは左から何番目ですか？\n",change-1);
    for(i=0;i<change;i++){
        printf("%d枚目：",i+1);
        scanf("%d", &leave[i]);
        while (leave[i]==place||leave[i]>7||leave[i]<1||leave[0]==leave[1]||leave[1]==leave[2]||leave[2]==leave[0]){
            printf("入力しなおしてください\n%d枚目：",i+1);
            scanf("%d", &leave[i]);
        }
        if(user[leave[i]-1].color==0) {*leftof_spe=*leftof_spe+1; appeared[60+user[leave[i]-1].number]=0;}
        else if(user[leave[i]-1].color>0) {*leftof_num=*leftof_num+1; appeared[(user[leave[i]-1].color-1)*10+user[leave[i]-1].number-1]=0;}
    }
    for(i=0;i<change;i++) user[leave[i]-1]=take[i];
}

void change(card user_table[36],int tablecards_user[9],int frag[9],int quag){
    int fragnum,num;
    int go;
    int i;
    printf("戦術カード「配置転換」を使います\n");
    printf("配置を変えたいカードは何番目のフラッグの何枚目のカードですか？スペース区切りで2つの数字を入力してください.\n(例えば, 上から7番目のフラッグの中央から2枚目のカードは 7 2 と入力してください\n");
    scanf("%d%d",&fragnum,&num);
    while(fragnum<1||fragnum>9||num>tablecards_user[fragnum-1]||frag[fragnum-1]!=2){
        printf("入力しなおしてください\n");
        scanf("%d%d",&fragnum,&num);
    }
    printf("今選んだカードを上から何番目のフラッグに移動しますか？\nゲームから除外する場合は 0 を入力してください\n");
    scanf("%d",&go);
    
    while(go<0||go>9||(go!=0&&((go==quag&&tablecards_user[go-1]==4)||(go!=quag&&tablecards_user[go-1]==3)||frag[go-1]!=2))){
        printf("入力しなおしてください\n");
        scanf("%d",&go);
    }
    tablecards_user[fragnum-1]--;
    card temp=user_table[(fragnum-1)*4+num-1];
    for(i=num-1;i<tablecards_user[fragnum-1];i++) user_table[(fragnum-1)*4+i]=user_table[(fragnum-1)*4+i+1];
    user_table[(fragnum-1)*4+tablecards_user[fragnum-1]].color=-2;
    user_table[(fragnum-1)*4+tablecards_user[fragnum-1]].open_or_close=1;
    
    if(go!=0){
        tablecards_user[go-1]++;
        user_table[(go-1)*4+tablecards_user[go-1]-1]=temp;
    }
}

void escape(card oppo_table[36],int tablecards_oppo[9],int frag[9]){
    int fragnum,num;
    int i;
    printf("戦術カード「脱走」を使います\n");
    printf("ゲームから除外する相手のカードは何番目のフラッグの何枚目のカードですか？スペース区切りで2つの数字を入力してください.\n(例えば, 上から7番目のフラッグの中央から2枚目のカードは 7 2 と入力してください\n");
    scanf("%d%d",&fragnum,&num);
    
    while(fragnum<1||fragnum>9||num>tablecards_oppo[fragnum-1]||frag[fragnum-1]!=2){
        printf("入力しなおしてください\n");
        scanf("%d%d",&fragnum,&num);
    }
    tablecards_oppo[fragnum-1]--;
    for(i=num-1;i<tablecards_oppo[fragnum-1];i++) oppo_table[(fragnum-1)*4+i]=oppo_table[(fragnum-1)*4+i+1];
    oppo_table[(fragnum-1)*4+tablecards_oppo[fragnum-1]].color=-2;
    oppo_table[(fragnum-1)*4+tablecards_oppo[fragnum-1]].open_or_close=1;
}

void betrayal(card user_table[36],int tablecards_user[9],card oppo_table[36],int tablecards_oppo[9],int frag[9],int quag){
    int fragnum,num;
    int go;
    int i;
    printf("戦術カード「裏切り」を使います\n");
    printf("相手から奪いたいカードは何番目のフラッグの何枚目のカードですか？スペース区切りで2つの数字を入力してください.\n(例えば, 上から7番目のフラッグの中央から2枚目のカードは 7 2 と入力してください\n");
    scanf("%d%d",&fragnum,&num);
    
    while(fragnum<1||fragnum>9||num>tablecards_oppo[fragnum-1]||frag[fragnum-1]!=2||oppo_table[(fragnum-1)*4+num-1].betlay!=1){
        printf("入力しなおしてください\n");
        scanf("%d%d",&fragnum,&num);
    }
    printf("今選んだカードを上から何番目のフラッグに移動しますか？\n");
    scanf("%d",&go);
    while(go<1||go>9||(go==quag&&tablecards_user[go-1]==4)||(go!=quag&&tablecards_user[go-1]==3)||frag[go-1]!=2){
        printf("入力しなおしてください\n");
        scanf("%d",&go);
    }
    tablecards_user[go-1]++;
    user_table[(go-1)*4+tablecards_user[go-1]-1]=oppo_table[(fragnum-1)*4+num-1];
    tablecards_oppo[fragnum-1]--;
    for(i=num-1;i<tablecards_user[fragnum-1];i++) oppo_table[(fragnum-1)*4+i]=oppo_table[(fragnum-1)*4+i+1];
    oppo_table[(fragnum-1)*4+tablecards_oppo[fragnum-1]].color=-2;
    oppo_table[(fragnum-1)*4+tablecards_oppo[fragnum-1]].open_or_close=1;
}
