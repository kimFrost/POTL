import { EvergreenPage } from './app.po';

describe('evergreen App', () => {
  let page: EvergreenPage;

  beforeEach(() => {
    page = new EvergreenPage();
  });

  it('should display welcome message', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('Welcome to app!');
  });
});
