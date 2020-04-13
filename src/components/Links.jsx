import React from 'react'
import { css } from 'glamor'
import githubImg from '../static/img/github.svg'
import tumblrImg from '../static/img/tumblr.svg'

const links = [
	{
		img: githubImg,
		url: 'https://github.com/doublejosh/FortuneTeller',
		label: 'GitHub',
	},
	{
		img: tumblrImg,
		url: 'https://doublejosh.com',
		label: 'Blog',
	},
]

export default () => (
	<section>
		<ul
			{...css({
				listStyle: 'none',
				margin: '5rem 0 10rem 0',
				padding: 0,
				textAlign: 'center',
			})}>
			{links &&
				links.map((link, i) => (
					<li
						key={link.url}
						{...css({
							display: 'inline',
							marginRight: '2rem',
						})}>
						<a href={link.url} target="_blank" rel="noopener noreferrer">
							<img
								src={link.img}
								alt={link.label}
								{...css({
									width: '40px',
								})}
							/>
						</a>
					</li>
				))}
		</ul>
	</section>
)
