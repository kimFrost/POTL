import { Injectable } from '@angular/core';
import {Subject} from 'rxjs/Subject';

import {BehaviorSubject} from 'rxjs/BehaviorSubject';
import { Observable } from 'rxjs';

import { Provider } from './provider';

@Injectable()
export class OverlayService {


  //@Output('update') change: EventEmitter<number> = new EventEmitter<number>();
  //new Subject<string>();
  //new ReplaySubject<string>(bufferSize);
  //new BehaviorSubject<string>("a");

  private subject = new Subject<Provider>();
  //public stream$ = this.subject.asObservable();

  constructor() { }

  public getProviderProfile(): Observable<any> {
    return this.subject.asObservable();
  }

  public setProviderProfile(provider: Provider): void {
    this.subject.next(provider);
  }

}
