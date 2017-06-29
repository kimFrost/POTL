namespace POTLModule {
    export class Resource {
        public Id: string = '';
        public Percentage: number = 100;
        public Value: number = 0;
        public Tags: Array<string> = [];
        public MaxValue: number = 50000;
        public Locked: boolean = false;
        public StoredIn: any
        constructor(
            Id:string,
            Tags?:Array<string>,
            MaxValue?:number,
        ) {
            this.Id = Id;
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
    }
}