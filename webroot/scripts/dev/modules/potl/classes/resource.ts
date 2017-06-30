namespace POTLModule {
    export class Resource {
        public Id: string = '';
        public Title: string = '';
        public Percentage: number = 100;
        public Value: number = 0;
        public Tags: Array<string> = [];
        public MaxValue: number = 50000;
        public Locked: boolean = false;
        public StoredIn: any
        constructor(
            Id:string,
            Title?:string,
            Tags?:Array<string>,
            MaxValue?:number,
        ) {
            this.Id = Title || this.Title;
            this.Title = Id;
            this.Tags = Tags || this.Tags ;
            this.MaxValue = MaxValue || this.MaxValue;
            this.Value = this.MaxValue;
        }

        public GetOwner(): void {

        }
        public Consume(): void {
            //void Consume(EConsumeType ConsumeType = EConsumeType::Undefined, bool bRemoveFromStorage = true);

        }
        public RemoveFromStorage(): void {

        }
        public Transfer(): void {
            //bool Transfer(UStorageComponent* Storage);

        }
        public Init(): void {

        }

        public add(amount: number): void {
            this.Value += amount;
            this.clampValue();
            this.updatePercentage();
        }
        public subtract(amount: number): void {
            this.Value -= amount;
            this.clampValue();
            this.updatePercentage();
        }
        public clampValue(): void {
            if (this.Value < 0) {
                this.Value = 0;
            }
            else if (this.Value > this.MaxValue) {
                this.Value = this.MaxValue;
            }
            this.Value = Math.ceil(this.Value);
        }
        public updatePercentage(): number {
            this.Percentage = Math.ceil(this.Value / this.MaxValue * 100);
            return this.Percentage;
        }
    }
}